var topico_umidade = new Array();
var topico_rele;
var i = 0;
var chart;
setInterval(function() {
client.subscribe('Umidade');
client.subscribe('vazao');
client.subscribe('statusrele');
client.onMessageArrived = function (message) {
if (message.destinationName=="Umidade"){
  document.getElementById("Topico1").innerHTML=(message.payloadString);
  console.log(message.payloadString);
  i = i +1;
  if (topico_umidade.indexOf(message.destinationName) < 0){
      topico_umidade.push(message.destinationName); //add new topic to array
      var y = topico_umidade.indexOf(message.destinationName); //get the index no
      //create new data series for the chart
      var newseries = {
              id: y,
              name: message.destinationName,
              data: []
              };
    chart.addSeries(newseries); //add the series
      };
  var y = topico_umidade.indexOf(message.destinationName); //get the index no of the topic from the array
  var myEpoch = new Date().getTime(); //get current epoch time
  var thenum = message.payloadString.replace( /^\D+/g, ''); //remove any text spaces from the message
  var plotMqtt = [myEpoch, Number(thenum)]; //create the array
  if (isNumber(thenum)) { //check if it is a real number and not text
    plot(plotMqtt, y);	//send it to the plot function
  };
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
  if (topico_umidade.indexOf(message.destinationName) < 0){
      topico_umidade.push(message.destinationName); //add new topic to array
      var y = topico_umidade.indexOf(message.destinationName); //get the index no
      //create new data series for the chart
      var newseries = {
              id: y,
              name: message.destinationName,
              data: []
              };
    chart.addSeries(newseries); //add the series
      };
  var y = topico_umidade.indexOf(message.destinationName); //get the index no of the topic from the array
  var myEpoch = new Date().getTime(); //get current epoch time
  var thenum = message.payloadString.replace( /^\D+/g, ''); //remove any text spaces from the message
  var plotMqtt = [myEpoch, Number(thenum)]; //create the array
  if (isNumber(thenum)) { //check if it is a real number and not text
    plot(plotMqtt, y);	//send it to the plot function
  };
}
};

}, 1000);
function isNumber(n) {
  return !isNaN(parseFloat(n)) && isFinite(n);
};
//function that is called once the document has loaded
function init() {
  //i find i have to set this to false if i have trouble with timezones.
  Highcharts.setOptions({
    global: {
      useUTC: false
    }
  });
  // Connect to MQTT broker
  client.connect(options);
};
//this adds the plots to the chart
  function plot(point, chartno) {
        var series = chart.series[0],
            shift = series.data.length > 20; // shift if the series is
                                             // longer than 20
        // add the point
        chart.series[chartno].addPoint(point, true, shift);
};
//settings for the chart
$(document).ready(function() {
    chart = new Highcharts.Chart({
        chart: {
            renderTo: 'container',
            defaultSeriesType: 'spline'
        },
        title: {
            text: 'Gráfico da Umidade do Solo'
        },
        subtitle: {
                              text: 'Teste 1'
                      },
        xAxis: {
            type: 'datetime',
            tickPixelInterval: 150,
            maxZoom: 20 * 1000
        },
        yAxis: {
            minPadding: 0.2,
            maxPadding: 0.2,
            title: {
                text: 'Value',
                margin: 80
            }
        },
        series: []
    });
});
