#include <Arduino.h>

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Alarm Clock Settings</title>
    <link rel="icon" type="image/x-icon" href="imageOn.png" />
    <style>
      body {
        background-color: black;
        color: white;
        font-family: system-ui;
        margin-right: 0px;
        margin-left: 0px;
        -webkit-tap-highlight-color: transparent;
      }

      .grid-container {
        display: grid;
        grid-template-columns: 70% 30%;
        border-bottom: solid;
        border-width: 2px;
        border-color: #941111;
        padding: 0px 25px 0px 25px;
      }

      #banner {
        display: grid;
        background-color: #212121;
        border-radius: 10px;
        margin: 20px 20px 30px 20px;
        padding: 10px 0px 10px 0px;
        justify-content: center;
        color: #4c4c4c;
      }

      #label1 {
        margin: auto;
        margin-top: 10px;
        font-size: 17px;
        font-weight: 400;
      }
     
      .time {
        font-size: 45px;
        font-weight: 500;
        margin-top: 30px;
        margin-bottom: 10px;
      }
      .days {
        font-size: 17px;
        font-weight: 400;
        margin-top: 10px;
        margin-bottom: 30px;
      }
      
      .switchdiv {
        align-content: center;
        text-align: right;
      }

      .switch {
        position: relative;
        display: inline-block;
        width: 60px;
        height: 34px;
      }

      .switch input {
        opacity: 0;
        width: 0;
        height: 0;
      }

      .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #212121;
        -webkit-transition: 0.4s;
        transition: 0.4s;
      }

      .slider:before {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        -webkit-transition: 0.4s;
        transition: 0.4s;
      }

      input:checked + .slider {
        background-color: #941111;
      }

      input:focus + .slider {
        box-shadow: 0 0 1px #941111;
      }

      input:checked + .slider:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(26px);
      }

      .slider.round {
        border-radius: 34px;
      }

      .slider.round:before {
        border-radius: 50%;
      }

      .modal {
        display: none;
        position: fixed;
        z-index: 1;
        left: 0;
        top: 0;
        width: 100%;
        height: 100%;
        overflow: auto;
        background-color: rgba(0, 0, 0, 0.4);
      }

      .modal-content {
        background-color: black;
        margin: 100px auto;
        padding: 20px;
        padding-top: 10px;
        border: 1px solid #941111;
        width: 340px;
        font-size: 20px;
      }

      .form-group {
        margin: 60px 0px 60px 0px;
      }
      .weekdays-form {
        display: grid;
        margin: 20px 0px 20px 0px;
      }

      .close {
        color: #aaa;
        float: right;
        font-size: 28px;
        font-weight: bold;
      }

      .close:hover,
      .close:focus {
        color: black;
        text-decoration: none;
        cursor: pointer;
      }

      .checkbox-form {
        display: block;
        position: relative;
        padding-left: 35px;
        margin-bottom: 12px;
        cursor: pointer;
        -webkit-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
      }

      .checkbox-form input {
        position: absolute;
        opacity: 0;
        cursor: pointer;
        height: 0;
        width: 0;
      }

      .checkmark-form {
        position: absolute;
        top: 3px;
        left: 0;
        height: 25px;
        width: 25px;
        background-color: #212121;
        border-radius: 8px;
      }

      .checkbox-form input:checked ~ .checkmark-form {
        background-color: #941111;
      }

      #alarmTime {
        background: #212121;
        border: none;
        border-radius: 20px;
        font-family: inherit;
        font-size: inherit;
        color: white;
        text-align: center;
      }

      #save-button {
        float: right;
        font: inherit;
        color: inherit;
        background: #941111;
        border: none;
        border-radius: 8px;
        padding: 5px 15px 5px 15px;
      }
    </style>
  </head>
  <body>
    <div id="banner">
      <img
        id="alarmimage"
        src="imageOff.png"
        alt="nope"
        style="width: auto; height: 120px; display: block; margin: auto"
      />
      <h6 id="label1">no active alarms</h6>
    </div>

    <div class="grid-container">
      <div onclick="openModal(0)">
        <h2 class="time" id="time-a1">00:00</h2>
        <h6 class="days" id="day-a1">Mon, Tue, Wed, Thu, Fri</h6>
      </div>

      <div class="switchdiv">
        <label class="switch">
          <input type="checkbox" id="alarm1" oninput="bannerResponse(0)" />
          <span class="slider round"></span>
        </label>
      </div>
    </div>

    <div class="grid-container">
      <div onclick="openModal(1)">
        <h2 class="time" id="time-a2">00:00</h2>
        <h6 class="days" id="day-a2">Mon, Tue, Wed, Thu, Fri</h6>
      </div>

      <div class="switchdiv">
        <label class="switch">
          <input type="checkbox" id="alarm2" oninput="bannerResponse(1)" />
          <span class="slider round"></span>
        </label>
      </div>
    </div>

    <div class="grid-container">
      <div onclick="openModal(2)">
        <h2 class="time" id="time-a3">00:00</h2>
        <h6 class="days" id="day-a3">Mon, Tue, Wed, Thu, Fri</h6>
      </div>

      <div class="switchdiv">
        <label class="switch">
          <input type="checkbox" id="alarm3" oninput="bannerResponse(2)" />
          <span class="slider round"></span>
        </label>
      </div>
    </div>
    <!-- Modal for alarms settings -->
    <div id="setting-modal" class="modal">
      <div class="modal-content">
        <span class="close">&times;</span>
        <div class="form-group">
          <div
            style="
              display: grid;
              grid-template-columns: 120px 130px;
              align-items: center;
            "
          >
            <label for="alarmTime">Alarm time:</label>
            <input type="time" id="alarmTime" required />
          </div>
          <label style="margin-top: 40px; display: inherit">Repeat:</label>
          <div class="weekdays-form">
            <label class="checkbox-form">
              <input type="checkbox" name="days" value="Mon" />
              <span class="checkmark-form"></span>Mon
            </label>
            <label class="checkbox-form"
              ><input type="checkbox" name="days" value="Tue" /><span
                class="checkmark-form"
              ></span>
              Tue</label
            >
            <label class="checkbox-form"
              ><input type="checkbox" name="days" value="Wed" /><span
                class="checkmark-form"
              ></span>
              Wed</label
            >
            <label class="checkbox-form"
              ><input type="checkbox" name="days" value="Thu" /><span
                class="checkmark-form"
              ></span>
              Thu</label
            >
            <label class="checkbox-form"
              ><input type="checkbox" name="days" value="Fri" /><span
                class="checkmark-form"
              ></span>
              Fri</label
            >
            <label class="checkbox-form"
              ><input type="checkbox" name="days" value="Sat" /><span
                class="checkmark-form"
              ></span>
              Sat</label
            >
            <label class="checkbox-form"
              ><input type="checkbox" name="days" value="Sun" /><span
                class="checkmark-form"
              ></span>
              Sun</label
            >
          </div>
          <button id="save-button" onclick="saveAlarm()">Save</button>
        </div>
      </div>
    </div>
  </body>

  <script>
    const alarmData = [];

    const DAYS = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];

    let modal = document.getElementById("setting-modal");
    let span = document.getElementsByClassName("close")[0];
    let currentAlarmID = 0;

    async function loadFromServer() {
      try {
        const response = await fetch("/loadData");
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        data.forEach(alarm => {
          alarm.days = reorderDays(alarm.days);
        });
        console.log("server data:", data);
        alarmData.push(...data);

        alarmData.forEach((_, i) => displayData(i));

        styleBanner();
        console.log("stored data:", alarmData);
      } catch (error) {
        console.error("Errore nel caricamento dei dati:", error);
      }
    }

    async function sendToServer(alarm_id) {
      try {
        const alarm = alarmData[alarm_id];
        const response = await fetch("/saveAlarm", {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify({ id: alarm_id, ...alarm }),
        });

        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }

        const result = await response.json();
        console.log("Server response:", result);
      } catch (error) {
        console.log("Errore durante l'invio dell'allarme:", error);
      }
    }

    function displayData(id) {
      const alarm = alarmData[id];

      const hh = alarm.hour.toString().padStart(2, "0");
      const mm = alarm.minute.toString().padStart(2, "0");
      const daysStr = alarm.days.map((i) => DAYS[i]).join(", ");

      document.querySelector(`#time-a${id + 1}`).textContent = hh + ":" + mm;
      document.querySelector(`#day-a${id + 1}`).textContent = daysStr;
      document.querySelector(`#alarm${id + 1}`).checked = alarm.enabled;
    }

    function styleBanner() {
      let statusA1 = document.querySelector("#alarm1").checked;
      let statusA2 = document.querySelector("#alarm2").checked;
      let statusA3 = document.querySelector("#alarm3").checked;

      if (statusA1 || statusA2 || statusA3) {
        document.querySelector("#banner").style.backgroundColor = "#9411114f";
        document.querySelector("#banner").style.color = "#941111";
        document.querySelector("#label1").textContent = "alarms active";
        document.querySelector("#alarmimage").src = "imageOn.png";
      } else {
        document.querySelector("#banner").style.backgroundColor = "#212121";
        document.querySelector("#banner").style.color = "#4c4c4c";
        document.querySelector("#label1").textContent = "no active alarms";
        document.querySelector("#alarmimage").src = "imageOff.png";
      }
    }

    function bannerResponse(id) {
      const alarm = alarmData[id];

      styleBanner();
      alarm.enabled = document.querySelector(`#alarm${id + 1}`).checked;
      sendToServer(id);
    }

    function openModal(id) {
      currentAlarmID = id;
      const alarm = alarmData[id];

      const hh = alarm.hour.toString().padStart(2, "0");
      const mm = alarm.minute.toString().padStart(2, "0");
      document.getElementById("alarmTime").value = hh + ":" + mm;

      const checkboxes = document.querySelectorAll('input[name="days"]');
      checkboxes.forEach((cb) => {
        const dayIndex = DAYS.indexOf(cb.value);
        cb.checked = alarm.days.includes(dayIndex);
      });

      modal.style.display = "block";
    }

    function saveAlarm() {
      const alarm = alarmData[currentAlarmID];

      const timeValue = document.querySelector("#alarmTime").value;
      const [hh, mm] = timeValue.split(":").map(Number);
      alarm.hour = hh;
      alarm.minute = mm;

      const checkboxes = document.querySelectorAll('input[name="days"]');
      const selectedDays = [];
      checkboxes.forEach((cb) => {
        if (cb.checked) {
          selectedDays.push(DAYS.indexOf(cb.value));
        }
      });
      alarm.days = selectedDays;
      alarm.enabled = true;

      displayData(currentAlarmID);
      styleBanner(currentAlarmID);

      console.log(alarmData);
      sendToServer(currentAlarmID);

      modal.style.display = "none";
    }

    span.onclick = function () {
      modal.style.display = "none";
    };

    window.addEventListener("DOMContentLoaded", () => {
      loadFromServer();
    });

    window.onclick = function (event) {
      if (event.target == modal) {
        modal.style.display = "none";
      }
    };

    function reorderDays(wdays) {
      return wdays.filter((d) => d !== 0).concat(wdays.includes(0) ? [0] : []);
    }
  </script>
</html>

)=====";