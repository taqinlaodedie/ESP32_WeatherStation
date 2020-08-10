#include "http.h"
#include "wifi.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "string.h"
#include "stdlib.h"
#include "cJSON.h"

extern EventGroupHandle_t wifi_event_group;
extern QueueHandle_t xQueue;
extern int CONNECTED_BIT;
static char recv_buf[1024];

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

// 提取接受到的JSON文件中的天气信息
static void parseWeatherElements(char *str, weatherContent_t *content)
{
    cJSON *root, *obj;

    /**
     * 收到的JSON文件示例：{"coord":{"lon":2.35,"lat":48.85},
     *  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01d"}],
     *  "base":"stations","main":{"temp":307.4,"feels_like":307.78,"temp_min":306.15,"temp_max":308.71,"pressure":1013,"humidity":33},
     *  "visibility":10000,"wind":{"speed":2.1,"deg":0},"clouds":{"all":8},"dt":1597074362,
     *  "sys":{"type":1,"id":6550,"country":"FR","sunrise":1597034207,"sunset":1597086910},
     *  "timezone":7200,"id":2988507,"name":"Paris","cod":200}
    */
    root = cJSON_Parse(str);
    obj = cJSON_GetObjectItem(root, "main");
    content->temp = (u16)((cJSON_GetObjectItem(obj, "temp")->valuedouble - 273.15) * 10);
    // printf("temp:%d\n", content->temp);
    content->pressure = (u16)(cJSON_GetObjectItem(obj, "pressure"))->valueint;
    // printf("pressure:%d\n", content->pressure);
    content->humidity = (u16)(cJSON_GetObjectItem(obj, "humidity"))->valueint;
    // printf("hum:%d\n", content->humidity);

    obj = cJSON_GetObjectItem(root, "wind");
    content->wind_speed = (u16)((cJSON_GetObjectItem(obj, "speed")->valuedouble) * 10);
    // printf("wind speed:%d\n", content->wind_speed);

    str = strstr(str, "{\"id");
    str = strtok(str, "]");
    // puts(str);
    root = cJSON_Parse(str);
    obj = cJSON_GetObjectItem(root, "main");
    strcpy((char *)content->type, (obj)->valuestring);
    // printf("main:%s\n", content->type);
}

void get_weather_task(void)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    weatherContent_t wMsg;
    message_t msg;
    char *buf;

    while(1) {
        /* Wait for the callback to set the CONNECTED_BIT in the
           event group.
        */
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
        printf("Connected to AP\n");

        int err = getaddrinfo(WEB_SERVER, "80", &hints, &res);

        if(err != 0 || res == NULL) {
            printf("DNS lookup failed err=%d res=%p\n", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.
           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        printf("DNS lookup succeeded. IP=%s\n", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        printf("... connected\n");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        
        /* Read HTTP response */
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);

        buf = strstr(recv_buf, "{");
        puts(buf);
        parseWeatherElements(buf, &wMsg);

        printf("... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);

        msg.ID = WEATHER_MSG;
        memcpy(&msg.data, &wMsg, sizeof(wMsg));
        xQueueSend(xQueue, (void *)&msg, 0);   // 将得到的天气信息发送至消息队列

        for(int countdown = 60; countdown >= 0; countdown--) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}