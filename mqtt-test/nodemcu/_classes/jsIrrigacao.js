      var dataField1; /* Thinkspeak data: Temperature DHT */
      var dataField2; /* Thinkspeak data: Humidity DHT */
      var topico_umidade = [];
      var i = 0;
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
          ['Label', 'Value'],
          ['SolidHum', 0],
          ['WaterFlow', 0],
        ]);
        var optionsT = {
          width: 600, height: 300,
          redFrom: 90, redTo: 100,
          yellowFrom:71, yellowTo: 90,
          greenFrom:59, greenTo: 71,
          min: 0, max: 100,
          minorTicks: 5
        };
        var chart = new google.visualization.Gauge(document.getElementById('chart_div'));
        chart.draw(data, optionsT);

        // Update of Gauges
        setInterval(function() {
          client.subscribe('Umidade');
          client.subscribe('vazao');
          client.onMessageArrived = function (message) {
            if (message.destinationName=="Umidade"){
                document.getElementById("Topico1").innerHTML=(message.payloadString);
                console.log(message.payloadString);
                topico_umidade+=message.payloadString;
                console.log(topico_umidade);
                console.log(i);
                console.log("tamanho:"+topico_umidade.length);
                data.setValue(0, 1, message.payloadString);
                chart.draw(data, optionsT);
                i = i +1;
            }
            else {
                document.getElementById("Topico2").innerHTML=(message.payloadString);
                console.log(message.payloadString);
                data.setValue(1, 1, message.payloadString);
                chart.draw(data, optionsT);
            }
          };

        }, 1000);
}
  /*    function getDataField1() {
            var topico;
            topico=('Umidade');
            client.subscribe(topico);
            client.onMessageArrived = function (message) {
              if (message.destinationName=="vazao"){
                return getDataField2();
              }
              else {
                document.getElementById("Topico1").innerHTML=(message.payloadString)
                dataField1=message.payloadString;
                console.log(message.payloadString);
                return dataField1;
              }
          };
      }

     function getDataField2() {
        var topico;
        topico=('vazao');
        client.subscribe(topico);
        client.onMessageArrived = function (message) {
          if (message.destinationName=="Umidade"){
              return getDataField1();
          }
          else {
              document.getElementById("Topico2").innerHTML=(message.payloadString)
              dataField2=message.payloadString;
              console.log(dataField2);
              return dataField2;
          }
      };
    }*/
