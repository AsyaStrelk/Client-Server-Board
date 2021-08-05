# Client-Server-Board

В проекте реализована передача информации между клиентом и микроконтроллером через сервер.
Возможны следующие действия:

- для клиента:
    - соединение с TCP-сервером по адресу введенному в текстовом поле;
    - изменение состояния выбранного светодиода на микроконтроллере;
- для сервера:
    - подключение к выбранному COM-порту и запуск сервера;
    - отключение от COM-порта и остановка сервера;
- для микроконтроллера:
    - отправка состояния диодов микроконтроллера по нажатию на одну из двух кнопок.

## Client Part ([code](https://github.com/AsyaStrelk/Client-Server-Board/tree/main/client))

## Server Part ([code](https://github.com/AsyaStrelk/Client-Server-Board/tree/main/TcpServerQt))

## Micro-controller Part ([code](https://github.com/AsyaStrelk/Client-Server-Board/tree/main/iar_remote_led))
