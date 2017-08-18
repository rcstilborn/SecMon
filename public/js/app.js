/**
 * Created by richard on 10/10/14.
 */


/*************************
 *  web sockets part
 *
 *************************/
var rooms = {};
var activeCameras = new Map();
var lastPage = null;
var webSocket = null;

function startWS(){
  webSocket = new WebSocket("ws://"+window.location.hostname+":9002");

  webSocket.onopen = function (event) {
    console.log("Connection opened!!");
    showMessage("Client: A connection to " + webSocket.url + " has been opened.<br />");
    $("#messages").css("background-color", "green");
//    restartAllCameras();
    console.log("lastPage=" + lastPage);
    if(lastPage) {
      console.log("Changing page to " + lastPage);
      window.location.hash = lastPage;
      window.location.reload();
      lastPage=null;
//      $(window).trigger('hashchange');
    }
  };

  webSocket.onmessage = function (event) {
    //var type = "", data = "";
    //if(event.data instanceof DOMString) type = "DOMString";
    //if(event.data instanceof Blob) {
      //console.log("Blob of size " + event.data.size + " and type " + event.data.type);
      //drawImageBinary(event.data);
      //drawFrame(event.data);
      //return;
    //} else if(event.data instanceof ArrayBuffer) {
      //console.log("ArrayBuffer");
      //drawFrame(event.data);
      //return;
    //}
    //type = "Blob";
    //else /*if(event.data instanceof String) {
     //type = "String";
     //data = JSON.parse(event);
     //}   else if(event.data instanceof Array)*/ {
      //type = event.data;
      //data = JSON.parse(event);
   //}

    showMessage("Got message of size: " + event.data.size + " and data " + event.data + "<br />");
    // + " msg: " + JSON.stringify(msg) + "<br />";
  };
  
  webSocket.onerror = function(e) {
    console.log(e);
    showMessage("Client: An error occured, see console log for more details.<br />");
    $("#messages").css("background-color", "red");
    checkWS();
  };

  webSocket.onclose = function(e) {
    console.log("Connection closed!! lastPage=" + lastPage);
    showMessage("Client: The connection to " + webSocket.url + " was closed. ["+e.code+(e.reason != "" ? ","+e.reason : "")+"]<br />");
    $("#messages").css("background-color", "red");
//    stopAllCameras();
    if(!lastPage || lastPage=='') {
      lastPage = window.location.hash;
      console.log("Stored lastPage=" + lastPage);
      window.location.hash = '';
      $(window).trigger('hashchange');
    }
    checkWS();
  };
};

function checkWS(){
  if(!webSocket || webSocket.readyState == 3) startWS();
};


//socket.on('deviceId', function(deviceId) {
//  localStorage.deviceId = deviceId;
//});
//
//socket.on('update', function(info) {
//  //console.log("Got an update for: " + info.roomName + " : " + info.data);
//  if(rooms[info.roomName])
//    rooms[info.roomName].updateFunc(info);
//  else
//    console.log("Got an update for a room we are not in!");
//});
//
//
//function send(roomName, data){
//  webSocket.send('{"room": "' + roomName + '", "data": ' + data + '}');
//}
//function joinRoom(roomName, updateFunc) {
//  send(roomName, '"join"');
//  if(rooms[roomName]) {
//    console.log("We are already in this room!");
//  } else {
//    rooms[roomName] = {'name': roomName, 'updateFunc': updateFunc};
//  }
//}
//
//function leaveRoom(roomName) {
//  send(roomName, '"leave"');
//  rooms[roomName] = null;
//}


/**********************************
 * window control
 *********************************/
$(window).on('hashchange', function(){
  // On every hash change the render function is called with the new hash.
  // This is how the navigation of our app happens.
  render(window.location.hash);
});

var map = {};

function registerModule(moduleName, buttonText, renderFunction) {
  console.log("Registering " + moduleName + " for button " + buttonText);
  if(map[moduleName]) {
    // Module already registered to ignore
    console.log("Module " + moduleName + " already registerd");
  } else {
    // Add to render map
    map[moduleName] = renderFunction;

    // Add button to navbar
    //newButton = $('<button />').addClass('navButton').text(buttonText).click(function() {
    //  window.location.hash = moduleName;
    //  $(window).trigger('hashchange');
    //});
    //newButton.appendTo('ul#navBar');

    newLink = $('<li>').append($('<a>').attr('href','#'+moduleName).text(buttonText));
    newLink.appendTo('ul#navBar');

  }
};

function render(url) {
  console.log("render() called with url=" + url);
  // This function decides what type of page to show
  // depending on the current url hash value.
  // Hide whatever page is currently shown.
  $('.page').removeClass('visible').trigger('hidePage');

  var temp = url.split('/')[0].substring(1);
  if(temp=='') temp = 'home-page';
  // Execute the needed function depending on the url keyword (stored in temp).
  if(map[temp]){
    map[temp]();
  }
  // If the keyword isn't listed in the above - render the error page.
  else {
    showErrorPage();
  }
}

function showErrorPage() {
  $('.error-page').addClass('visible')
}

function showMessage(message) {
    document.getElementById("messages").innerHTML = message;
}

/**********************************
 * Home Page
 *
 *********************************/
registerModule('home-page', 'Home', function() {
  $('.home-page').addClass('visible');
});

//screen.orientation.lock('landscape');
//window.location.hash = '';

startWS();
setInterval(checkWS, 5000);


/**********************************
 * Single Scene Page
 *
 *********************************/
var singleSceneOnTop = false;
var video0 = document.getElementById("video0");

$('.singleScene').on('hidePage', function() {
  // Hiding page so leave the room
  if(singleSceneOnTop) {
    singleSceneOnTop = false;
    //leaveRoom('singleScene');
    stopCamera("video0");
  }
});

var showSingleScenePage = function(){
  // Showing the page so join the room
  var page = $('.singleScene');
  page.addClass('visible');
  singleSceneOnTop = true;
  //$('#singleScene-data').empty();
  startCamera('video0', '001.mjpeg');

  //joinRoom('singleScene', updateSingleScene);
};

registerModule('singleScene', 'One', showSingleScenePage);

//function updateSingleScene(data) {
//  document.getElementById("messages").innerHTML = "Got this for updateSingleScene: " + JSON.stringify(data) + "<br />";
//};



/**********************************
 * 2 x 2 Grid Page
 *
 *********************************/
var twoByTwoOnTop = false;
var video0 = document.getElementById("video0");

$('.twoByTwo').on('hidePage', function() {
  // Hiding page so leave the room
  if(twoByTwoOnTop) {
    twoByTwoOnTop = false;
    //leaveRoom('twoByTwo');
    stopCamera('video1');
    stopCamera('video2');
    stopCamera('video3');
    stopCamera('video4');
  }
});

var showTwoByTwoPage = function(){
  // Showing the page so join the room
  var page = $('.twoByTwo');
  page.addClass('visible');
  twoByTwoOnTop = true;
  //$('#twoByTwo-data').empty();
startCamera('video1', '001.mjpeg');
startCamera('video2', '002.mjpeg');
startCamera('video3', '003.mjpeg');
startCamera('video4', '004.mjpeg');

  //joinRoom('twoByTwo', updateTwoByTwo);
};

registerModule('twoByTwo', 'Four', showTwoByTwoPage);

//function updateTwoByTwo(data) {
//  document.getElementById("messages").innerHTML = "Got this for updateTwoByTwo: " + JSON.stringify(data) + "<br />";
//};



/**********************************
 * Cameras Functions
 *
 *********************************/
function startCamera(cameraName, streamName) {
  var height = $('#'+cameraName).parent().parent().height();
  $('#'+cameraName).parent().css({'max-height' : height});
  $('#'+cameraName).attr("src", streamName);
  activeCameras.set(cameraName, streamName);
};

function stopCamera(cameraName) {
  $('#'+cameraName).attr("src", "");
  activeCameras.delete(cameraName);
};

//                  <img class="scene-img" id="video1" />

function stopAllCameras() {
  if(activeCameras.size == 0) return;
  showMessage("Stopping " + activeCameras.size + " cameras..." + "<br />");
  activeCameras.forEach(function(value, key, map) {
    $('#'+key).removeAttr('src');  
//    $('#'+key).remove();  
  });
};

function startAllCameras() {
  if(activeCameras.size == 0) return;
  showMessage("Starting " + activeCameras.size + " cameras..." + "<br />");
  activeCameras.forEach(function(value, key, map) {
//    $('#'+key).attr("src", value);  
    $('#'+key).attr("src", value);  
  });
};

function restartAllCameras() {
  if(activeCameras.size == 0) return;
  showMessage("Restarting " + activeCameras.size + " cameras..." + "<br />");
  activeCameras.forEach(function(value, key, map) {
//    $('#'+key).removeAttr('src');  
    var parent = $('#'+key).parent();
    $('#'+key).remove();
    parent.append("<img class=\"scene-img\" id=\"" + key + "\" src=\"" + value + "\" />");  
  });

//  showMessage("Starting " + activeCameras.size + " cameras..." + "<br />");
//  activeCameras.forEach(function(value, key, map) {
//    $('#'+key).attr("src", value);  
//  });
};







$(window).trigger('hashchange');










/**********************************
 * Cameras Page
 *
 *********************************/
//var camerasOnTop = false;
//var stream0 = document.getElementById("stream0");
//var container = document.getElementById("container");
//var context0 = stream0.getContext("2d");

//$('.cameras').on('hidePage', function() {
  // Hiding page so leave the room
//  if(camerasOnTop) {
//    camerasOnTop = false;
//    leaveRoom('cameras');
//  }
//});
//
//var showCamerasPage = function(){
//  // Showing the page so join the room
//  var page = $('.cameras');
//  var c1 = $('#one_camera');
//  //var c4 = $('#four_camera');
//  //c1.addClass('visible');
//  //c4.addClass('hidden');
//  page.addClass('visible');
//  camerasOnTop = true;
//  $('#cameras-data').empty();
//  joinRoom('cameras', updateCameraList);
//};
//
////registerModule('cameras', 'Cameras', showCamerasPage);
//
//function updateCameraList(data) {
//  document.getElementById("messages").innerHTML = "Got this camera list: " + JSON.stringify(data) + "<br />";
//
//  // Add button to navbar
//  newButton = $('<button />').addClass('cameraButton').text(data.data[0]).click(function() {
//  });
//  newButton.appendTo('ul#cameraList');
//
//}
//
//function drawImageBinary(blob) {
//    var bytes = new Uint8Array(blob);
//    console.log('drawImageBinary (bytes.length): ' + bytes.length);
//
//    var imageData = context0.createImageData(stream0.width, stream0.height);
//
//    for (var i=8; i<imageData.data.length; i++) {
//        imageData.data[i] = bytes[i];
//    }
//    context0.putImageData(imageData, 0, 0);
//
//    var img = document.createElement('img');
//    img.height = stream0.height;
//    img.width = stream0.width;
//    img.src = stream0.toDataURL();
//}
//
//function drawFrame(image) {
//  var urlCreator = window.URL || window.webkitURL;
//  var imageUrl = urlCreator.createObjectURL( image );
//  //console.log(imageUrl);
//  var img = new Image();
//  img.onload = function () {
//    context0.drawImage(img, 0, 0);
//  };
//  img.src = imageUrl;
//};
//
//
/**********************************
 * System Status part
 *
 *********************************/
//    var systemStatusOnTop = false;
//    var cpuSpeedHistory0 = new CBuffer(100);
//    var cpuSpeedHistory1 = new CBuffer(100);
//    var cpuSpeedHistory2 = new CBuffer(100);
//    var cpuSpeedHistory3 = new CBuffer(100);
//
//    $('.system-status').on('hidePage', function() {
//      // Hiding page so leave the room
//      if(systemStatusOnTop) {
//        systemStatusOnTop = false;
//        leaveRoom('system-status');
//      }
//    });
//
//    var showSystemStatusPage = function(){
//      // Showing the page so join the room
//      var page = $('.system-status');
//      page.addClass('visible');
//      systemStatusOnTop = true;
//      $('#system-status-data').empty();
//      joinRoom('system-status', updateCPUData);
//    };
//
//    registerModule('system-status', 'System Status', showSystemStatusPage);
//
//    function updateCPUData(data) {
//      // One element or all?
//      if((data.data.cpu0).constructor === Array) {
//        console.log("Got all history");
//        cpuSpeedHistory0.fill(data.data.cpu0);
//        cpuSpeedHistory1.fill(data.data.cpu1);
//        cpuSpeedHistory2.fill(data.data.cpu2);
//        cpuSpeedHistory3.fill(data.data.cpu3);
//      } else {
//        console.log("Got one item");
//        cpuSpeedHistory0.push(data.data.cpu0);
//        cpuSpeedHistory1.push(data.data.cpu1);
//        cpuSpeedHistory2.push(data.data.cpu2);
//        cpuSpeedHistory3.push(data.data.cpu3);
//      }
//      myBarChart.removeData();
//      myBarChart.addData([data.data.cpu0, data.data.cpu1, data.data.cpu2, data.data.cpu3],'');
//      //updateCPUChart();
//
//    }
//
//    var chartData = {
//      labels:
//       ['1800','.','.','.','.','.','.','.','.','.','1790','.','.','.','.','.','.','.','.','.',
//        '1780','.','.','.','.','.','.','.','.','.','1770','.','.','.','.','.','.','.','.','.',
//        '1760','.','.','.','.','.','.','.','.','.','1750','.','.','.','.','.','.','.','.','.',
//        '1740','.','.','.','.','.','.','.','.','.','1730','.','.','.','.','.','.','.','.','.',
//        '1720','.','.','.','.','.','.','.','.','.','1710','.','.','.','.','.','.','.','.','.'],
//      datasets: [
//        {
//          label: "CPU 0",
//          fillColor: "rgba(220,220,220,0.5)",
//          strokeColor: "rgba(220,220,220,0.8)",
//          highlightFill: "rgba(220,220,220,0.75)",
//          highlightStroke: "rgba(220,220,220,1)",
//          data: [99]
//        },
//        {
//          label: "CPU 1",
//          fillColor: "rgba(151,187,205,0.5)",
//          strokeColor: "rgba(151,187,205,0.8)",
//          highlightFill: "rgba(151,187,205,0.75)",
//          highlightStroke: "rgba(151,187,205,1)",
//          data: []
//        },
//        {
//          label: "CPU 2",
//          fillColor: "rgba(151,187,205,0.5)",
//          strokeColor: "rgba(151,187,205,0.8)",
//          highlightFill: "rgba(151,187,205,0.75)",
//          highlightStroke: "rgba(151,187,205,1)",
//          data: []
//        },
//        {
//          label: "CPU 3",
//          fillColor: "rgba(151,187,205,0.5)",
//          strokeColor: "rgba(151,187,205,0.8)",
//          highlightFill: "rgba(151,187,205,0.75)",
//          highlightStroke: "rgba(151,187,205,1)",
//          data: []
//        }
//      ]
//    };
//
//// Get the context of the canvas element we want to select
//    var ctx = document.getElementById("myChart").getContext("2d");
//    var myBarChart = new Chart(ctx).Bar(chartData);
//
//    function updateCPUChart() {
//      //console.log("Chart data " + chartData.datasets[0].data);
//      console.log("Chart data " + myBarChart.datasets[0].data);
//      myBarChart.datasets[0].data = cpuSpeedHistory0.reverse().toArray();
//      myBarChart.datasets[1].data = cpuSpeedHistory1.reverse().toArray();
//      myBarChart.datasets[2].data = cpuSpeedHistory2.reverse().toArray();
//      myBarChart.datasets[3].data = cpuSpeedHistory3.reverse().toArray();
//      //console.log("Data " + cpuSpeedHistory0.reverse().toArray());
//      console.log("Chart data " + myBarChart.datasets[0].data);
//      myBarChart.update();
//    }


/**********************************
 * Camera insert
 *
 *********************************/
//image = "data:cameraList/png;base64," + JSON.parse(data);
//   drawFrame(image);
//}
//
