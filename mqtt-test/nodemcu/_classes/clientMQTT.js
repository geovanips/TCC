//require('./mqttws31');
var MQTTWS_BROKER = "IP do broker"; // IP Broker MQTT (Meu Servidor)
var WS_PORT = 80; // Porta de WebSockets para Comunicação
var client_id = "client_" + parseInt(Math.random() * 100000, 10); //gera id aleatorio para cada cliente conectado
var client = new Paho.MQTT.Client(MQTTWS_BROKER, WS_PORT,client_id); // Instancia o Cliente MQTT

client.onConnectionLost = function (responseObject) {
	console.log("Status: " + responseObject.errorMessage);
};
client.onMessageArrived = function (message) {
	console.log('Tópico: ', message.destinationName, ' | Mensagem: ', message.payloadString);
};
var options = {
	timeout: 3,
	onSuccess: function () {// se a conexão for bem sucessida manda uma mensagem pro console e pode fazer subscription e mandar mensagens
		console.log("Conectado com o Broker MQTT");
		console.log("Host="+ MQTTWS_BROKER + ", port=" + WS_PORT);
	    //client.subscribe('led/1', {qos: 1}); // Assina o Tópico led/1
	},
	onFailure: function (message) {//caso a conexão falhe, mostra a mensagem de erro
		console.log("Connection failed: " + message.errorMessage);
	}
};
//inicia conexão
function init() {
	client.connect(options); // Conecta ao Broker MQTT
}
