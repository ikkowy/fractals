web_socket = new WebSocket("ws://localhost:8080/view");

web_socket.onopen = function() {
    console.log("web socket opened");
}

web_socket.onmessage = function() {
    console.log("web socket message received");
}

web_socket.onclose = function() {
    console.log("web socket closed");
}
