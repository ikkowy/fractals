web_socket = new WebSocket("ws://localhost:8080/view");

web_socket.onopen = function() {
    console.log("web socket opened");
}

web_socket.onmessage = function(event) {
    var message = event.data
    console.log(typeof(message));
    console.log(message)

    if (typeof message === "string") {
        var json = JSON.parse(message);
        if ("action" in json) {
            if (json.action === "info") {
                if ("nodes_count" in json) {
                    document.getElementById("nodes_count").innerHTML = json.nodes_count;
                }
            }
        }
    }
}

web_socket.onclose = function() {
    console.log("web socket closed");
}
