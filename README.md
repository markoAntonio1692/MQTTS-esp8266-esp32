# MQTTS-esp8266-esp32
Este es un proyecto realizado para conexión MQTTS hacia broker mosquitto instalado en ubuntu 16.04 + autenticacion
Referencia:
https://primalcortex.wordpress.com/2016/03/31/mqtt-mosquitto-broker-with-ssltls-transport-security/

http://www.iotsharing.com/2017/08/how-to-use-esp32-mqtts-with-mqtts-mosquitto-broker-tls-ssl.html

https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-16-04


#UBUNTu SERVER 16.04

#INSTALAR MOSQUITTO BROKER:
sudo apt-get install mosquitto mosquitto-clients

#Tener en cuenta fichero de configuracion guardado en repositorio "default.conf"

#comando mosquitto MQTTS
 mosquitto_sub --cafile /etc/mosquitto/certs/ca.crt -h localhost -t "outTopic"  -p 8883 -u "marko" -P "marcoantonio"
