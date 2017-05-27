      var dataField1; /* Thinkspeak data: Temperature DHT */
      var dataField2; /* Thinkspeak data: Humidity DHT */
      var topico_umidade = [];
      var topico_rele;
      var i = 0;
setInterval(function() {
  client.subscribe('Umidade');
  client.subscribe('vazao');
  client.subscribe('statusrele');
  client.onMessageArrived = function (message) {
    if (message.destinationName=="Umidade"){
        document.getElementById("Topico1").innerHTML=(message.payloadString);
        console.log(message.payloadString);
        topico_umidade+=""+message.payloadString;
        console.log(topico_umidade);
        console.log(i);
        console.log("tamanho:"+topico_umidade.length);
        i = i +1;
    }
    else if(message.destinationName=="statusrele"){
      topico_rele=message.payloadString;
      console.log(topico_rele);
      x=document.getElementById("botaoStatusRele");
      if (topico_rele == '1') {
        x.className="botaoReleLigado";
        document.getElementById("botaoStatusRele").innerHTML="Ligado";
    }
      else {
        x.className="botaoReleDesligado";
        document.getElementById("botaoStatusRele").innerHTML="Desligado";
      }
    }
    else {
        document.getElementById("Topico2").innerHTML=(message.payloadString);
        console.log(message.payloadString);
    }
  };

}, 1000);
