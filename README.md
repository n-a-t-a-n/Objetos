## Descrição:
Projeto de dispenser automático de medicamentos. Faz uso de uma placa NodeMCU e uma aplicação local de react para controlar o estado de um servo-motor.

## Documentação:
O código consiste na utilização das bibliotecas ESP8266WiFi, PubSubClient e NTPClient para comunicar-se com o servidor MQTT:

- void recebePacote (função): Função Boiler Plate que recebe uma payload de um tópico definido e mapea cada caractere em uma string (não retornada), este função é chamada automaticamente, e deve ser atribuída a propriedade "setCallback" do objeto MQTT instanciado.

- void enviaMessage (função): Função Boiler Plate que recebe uma string como argumento, que será enviada para um tópico definido, função sem retorno, o status de conexão de envio do pacote deve ser monitorado via serial.

- formattedTime (variável): Função recebe o horário em formato HH:MM:SS de um servidor NTP, esta comando o horário de atuação do servo-motor, e deve ter uma comparação "hard-coded" definida nos laços condicionais "if"
