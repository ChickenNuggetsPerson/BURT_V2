

// Let us open a web socket
var ws = new WebSocket("ws://localhost:7071/vexrobotics.vexcode/device");
				
ws.onopen = function() {
   ws.send("Connected Web Editor");
};
 
ws.onmessage = async function (evt) { 
    var recievedData = await evt.data.text();
    let data = recievedData.split("\n")
    let messages = []
    data.forEach(message => {
        let newMessage = message.replace("\r", "")
        if (newMessage != "") {
            messages.push(newMessage)
        }
    });
    if (messages == []) {return}
    messages.forEach(message => {
        if (message.startsWith("__")) {
            try {
                handleData(message)
            } catch (err) {}
        } else {
            handleLog(message)
        }
    });
};
 
ws.onclose = function() { 
   alert("Connection is closed..."); 
};

let logs = []
function handleLog(message) {
    logs.push(message)
    console.log("Message", message)
    
    let text = ""
    logs.forEach(log => {
        text += log
        text += "\n"
    })
    
    document.getElementById("logs").innerText = text

}
function handleData(message) {
    const args = message.split(' ');
    console.log(args)
    if ((args[1] == "TP" || args[1] == "DP") || args[1] == "T") {
        handlePIDData(args[1], args[2]);
        return;
    }
    let percentVal = args[2] + "%"
    document.getElementById(args[1]).style.width = percentVal
    document.getElementById(args[1]).innerText = percentVal

    document.getElementById(args[1]).classList.remove("bg-danger")
    document.getElementById(args[1]).classList.remove("bg-warning")
    document.getElementById(args[1]).classList.remove("bg-success")

    if (args[1] == "bat") {
        colorBasedOnBattery(args[1], args[2])
    } else {
        colorBasedOnTemp(args[1], args[2])
    }
} 

function colorBasedOnTemp(id, temp) {
    if (temp > 80) {
        document.getElementById(id).classList.add("bg-danger")
        return;
    }
    if (temp > 50) {
        document.getElementById(id).classList.add("bg-warning")
        return;
    }
    if (temp > 0) {
        document.getElementById(id).classList.add("bg-success")
        return;
    }
}
function colorBasedOnBattery(id, batt) {
    if (batt > 75) {
        document.getElementById(id).classList.add("bg-success")
        return;
    }
    if (batt > 35) {
        document.getElementById(id).classList.add("bg-warning")
        return;
    }
    if (batt > 0) {
        document.getElementById(id).classList.add("bg-danger")
        return;
    }
}

function handlePIDData(graphLabel, value) {

    let percent = Math.abs((( value / 12 ) * 100) / 2);
    console.log(percent)

    if (value > 0) {
        document.getElementById(graphLabel + "1").style.width = "50%"
        document.getElementById(graphLabel + "2").style.width = percent + "%"
    } else {
        document.getElementById(graphLabel + "1").style.width = ( 50 - percent ) + "%"
        document.getElementById(graphLabel + "2").style.width = percent + "%"

    }
}


handleData("__ RMA 100");
handleData("__ RMB 100");
handleData("__ LMA 100");
handleData("__ LMB 100");
handleData("__ bat 100");
handleData("__ watt 100");

handleData("__ T 100000");
handleData("__ DP 100000");
handleData("__ TP 100000");


