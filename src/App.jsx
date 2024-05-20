import { useState, useEffect } from 'react';
import './App.css';
import mqtt from 'mqtt';

function App() {

    const [receivedMessage, setReceivedMessage] = useState('');

    // const sendMessage = (arg) => {
    //   const brokerUrl = 'mqtt://test.mosquitto.org';
    //   const topic = 'TESTE_COM_NTN';
    //   const message = arg;
    //   const client = mqtt.connect(brokerUrl, {
    //     connectTimeout: 5000,
    //     port: 8080,
    //     path: "/mqtt",  
    //   });

    //   client.on('connect', () => {
    //     client.publish(topic, message);
    //     client.end(); 
    //   });

    // };


    useEffect(() =>{

      const brokerUrl = 'mqtt://test.mosquitto.org';
      const topic = 'TESTE_COM_NTN';
  
      const client = mqtt.connect(brokerUrl, {
        connectTimeout: 5000,
        port: 8080,
        path: "/mqtt",  
      });


      client.on('connect', () => {
        console.log('Conectado');
        client.subscribe(topic, (error) => {
          if (error) {
            console.error(error);
          } else {
            console.log(topic);
          }
        });
      });


      client.on('message', (topic, message) => {
        const receivedMessage = message.toString();
        console.log('Mensagem recebida:', receivedMessage);
        setReceivedMessage(receivedMessage);
      });

    }, [])

    return (
      <div className="App">
        <h1>Há comprimidos no compartimento ? {receivedMessage}</h1>
      </div>
    );
  }


export default App;
