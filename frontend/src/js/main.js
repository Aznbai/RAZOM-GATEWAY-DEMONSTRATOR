// Load Styles
import '../scss/main.scss';
// Load Bootstrap init
import { initBootstrap } from "./bootstrap.js";
// // Loading bootstrap with optional features
import * as bootstrap from 'bootstrap'
initBootstrap({
  tooltip: true,
  popover: false,
  toasts: false,
});
// HELPER FUNCTIONS
var $id = function(id) {
  return document.getElementById(id); //define id selector shorthand
};
// ON DOCUMENT READY
window.onload = function(event) {
  requestStatus();
};
// // ADD EVENT LISTENERS TO NUMBER ALL INPUTS
// const divs = document.querySelectorAll('input[type=number]'); //get all number inputs
// divs.forEach(el => el.addEventListener('change', e => { //bind e listeners to each one
//   // console.log("e listener fired at ID: \n" + e.target.getAttribute("id"));
//   var node = parseInt(e.target.getAttribute("data-node")); //node number
//   var param = e.target.getAttribute("data-param"); //parameter name, as saved inside the struct_message on the ESP32
//   var value = parseInt(e.target.value);
//   var jsonValue = JSON.stringify({ "node": node, "param": param, "value": value });
//   sendJSONVal(jsonValue);
// }));
const modeTabNamePartials = ["RollPitchMap-tab", "AccelMap-tab", "ZKnobMap-tab"]; //mode tab IDs look like "node0RollPitchMap-tab" 
// // NODE0 ADD EVENT LISTENERS TO NUMBER, SELECT AND CHECKBOX INPUTS
// document.getElementById('node0Mode').addEventListener('change', function() {
//   var selectedTab = "node0"+modeTabNamePartials[this.value];
//   console.log("event listender added to tab selector: " + selectedTab);
//   var tabToShow = new bootstrap.Tab(document.getElementById(selectedTab));
//   tabToShow.show(); // Using Bootstrap 5 method to show the tab
// });
//  ADD TAB CHANGE LOGIC EVENT LISTENERS TO MODE SELECTORS
document.getElementById("node0Mode").addEventListener('change', function() {
  var selectedTab = "node0" + modeTabNamePartials[this.value];
  console.log("event listender of mapping mode tab fired: " + selectedTab);
  var tabToShow = new bootstrap.Tab(document.getElementById(selectedTab));
  tabToShow.show(); // Using Bootstrap 5 method to show the tab
});
document.getElementById("node1Mode").addEventListener('change', function() {
  var selectedTab = "node1" + modeTabNamePartials[this.value];
  console.log("event listender of mapping mode tab fired: " + selectedTab);
  var tabToShow = new bootstrap.Tab(document.getElementById(selectedTab));
  tabToShow.show(); // Using Bootstrap 5 method to show the tab
});
document.getElementById("node2Mode").addEventListener('change', function() {
  var selectedTab = "node2" + modeTabNamePartials[this.value];
  console.log("event listender of mapping mode tab fired: " + selectedTab);
  var tabToShow = new bootstrap.Tab(document.getElementById(selectedTab));
  tabToShow.show(); // Using Bootstrap 5 method to show the tab
});
document.getElementById("node3Mode").addEventListener('change', function() {
  var selectedTab = "node3" + modeTabNamePartials[this.value];
  console.log("event listender of mapping mode tab fired: " + selectedTab);
  var tabToShow = new bootstrap.Tab(document.getElementById(selectedTab));
  tabToShow.show(); // Using Bootstrap 5 method to show the tab
});
//  ADD VALUE CHANGE EVENT LISTENERS TO ALL OTHER NUMBER, SELECT  AND CHECKBOX INPUTS
const numberInputs = document.querySelectorAll('input[type=number]');
numberInputs.forEach(el => el.addEventListener('change', handleInputChange));
const selectElements = document.querySelectorAll('select');
selectElements.forEach(el => el.addEventListener('change', handleInputChange));
// Add event listeners to checkbox inputs
const checkboxInputs = document.querySelectorAll('input[type=checkbox]');
checkboxInputs.forEach(el => el.addEventListener('click', handleInputChange));
// Handler for input changes
function handleInputChange(e) {
  var node = parseInt(e.target.getAttribute("data-node")); // Node number
  var param = e.target.getAttribute("data-param"); // Parameter name
  var value;
  // Check the type of input and set value accordingly
  if (e.target.type === "checkbox") {
    value = e.target.checked; // Boolean value for checkboxes
  } else {
    value = parseInt(e.target.value); // Numeric value for number inputs and selects
  }
  var jsonValue = JSON.stringify({ "node": node, "param": param, "value": value });
  sendJSONVal(jsonValue);
}
// SEND JSON PARAMETER CHANGE VIA POST REQUEST
function sendJSONVal(value) { // Post JSON
  console.log("sending value: ");
  console.log(value);
  const xhr = new XMLHttpRequest(); // New HttpRequest instance 
  xhr.open("POST", "/set-param", true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      var json = JSON.parse(xhr.responseText);
      console.log(json);
    }
  };
  xhr.send(value);
}
// GET JSON GATEWAY STATUS VIA POST REQUEST
// function requestStatus() {
//   console.log("requesting JSON status object");
//   var xhr = new XMLHttpRequest();
//   xhr.open("POST", "/status", true);
//   xhr.setRequestHeader("Content-Type", "application/json");
//   xhr.onreadystatechange = function() {
//     if (xhr.readyState === 4 && xhr.status === 200) {
//       var json = JSON.parse(xhr.responseText);
//       console.log(json);
//     }
//   };
//   var data = JSON.stringify({ "give": "dataz" });
//   xhr.send(data);
// }
function requestStatus() {
  console.log("requesting JSON status object");
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/status", true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      var json = JSON.parse(xhr.responseText);
      console.log(json);
      // Iterate over each node in the JSON object
      for (var nodeKey in json) {
        if (json.hasOwnProperty(nodeKey)) {
          var node = json[nodeKey];
          // Iterate over each parameter within the node
          for (var paramKey in node) {
            if (node.hasOwnProperty(paramKey)) {
              // Construct the ID of the corresponding input field
              var inputId = nodeKey + paramKey.charAt(0).toUpperCase() + paramKey.slice(1);
              var inputElement = document.getElementById(inputId);
              // Update the input field's value if the element is found
              if (inputElement) {
                console.log("ui element FOUND: " + inputId + " updating to: " + node[paramKey])
                // Handle checkbox inputs
                if (inputElement.type === "checkbox") {
                  inputElement.checked = Boolean(node[paramKey]);
                }
                // Handle select inputs
                else if (inputElement.tagName === "SELECT") {
                  for (var i = 0; i < inputElement.options.length; i++) {
                    if (inputElement.options[i].value == node[paramKey].toString()) {
                      // SET MODE SELECTOR 
                      inputElement.options[i].selected = true;
                      //SHOW CORRESPONDING MODE SELECTOR TAB
                      var selectedTab = nodeKey + modeTabNamePartials[i];
                      console.log("selectedm mode mapping tab: " + selectedTab);
                      var tabToShow = new bootstrap.Tab(document.getElementById(selectedTab));
                      tabToShow.show(); // Using Bootstrap 5 method to show the tab
                      break;
                    }
                  }
                }
                // Handle other input types
                else {
                  inputElement.value = node[paramKey];
                }
              } else {
                console.log("ui element not found! " + inputId);
              }
            }
          }
        }
      }
    }
  };
  var data = JSON.stringify({ "give": "dataz" });
  xhr.send(data);
}
// function updateValue(e) {
//   sendUiVal("node", e.target.value);
// }
// node1RollTargetMin
// node1RollTargetMax
// node1RollMapMin
// node1RollMapMax
// function setBPM() {
//   stateUI.bpm = $id("bpm").value;
//   sendUiVal("bpm", stateUI.bpm);
// }
// const json = {
//   type: 'div',
//   props: { id: 'hello', class: "foo" },
//   children: [
//     { type: 'h1', children: 'HELLO' },
//     { type: 'p', children: [{ type: 'span', props: { class: "bar" }, children: 'World' }] }
//   ]
// };
// var stateUI = {
//   play: false,
//   bpm: 120,
//   drumCh: 1,
//   chordCh: 2,
//   arpCh: 3,
//   drumPartsCount: 6,
//   partBitMask: ["16", "16", "16", "16", "16", "16"],
//   partCh: [16, 16, 16, 16, 16, 16]
// };
// var partDiv,
//   chordStepDiv,
//   chordStepInput;
// function getRemoteState() {
//   sending = true;
//   const xhr = new XMLHttpRequest();
//   xhr.open("POST", "/state", true);
//   xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
//   xhr.onreadystatechange = function() {
//     if (xhr.readyState == 4 && xhr.status == 200) {
//       if (!sending) {
//         setLocalState(xhr.responseText);
//       }
//     }
//   }
//   xhr.send("");
//   sending = false;
// }
// function setLocalState(response) {
//   remoteState = JSON.parse(response);
//   console.log(remoteState);
//   // SET PLAY
//   if (remoteState.playOn == 0) {
//     stateUI.play = false;
//     $id("play").checked = false;
//   } else {
//     stateUI.play = true;
//     $id("play").checked = true;
//   }
//   // SET BPM
//   stateUI.bpm = remoteState.bpm;
//   $id("bpm").value = stateUI.bpm;
//   // SET DRUM CH
//   stateUI.drumCh = remoteState.drumCh;
//   $id("drumCh").value = stateUI.drumCh;
//   // SET CHORD CH
//   stateUI.chordCh = remoteState.chordCh;
//   $id("chordCh").value = stateUI.chordCh;
//   // SET ARPCH
//   // stateUI.arpCh = remoteState.arpCh;
//   // $id("arpCh").value = stateUI.arpCh;
//   // SET PART LENGTH
//   stateUI.partLength[0] = remoteState.drumMask0.length;
//   stateUI.partLength[1] = remoteState.drumMask1.length;
//   stateUI.partLength[2] = remoteState.drumMask2.length;
//   stateUI.partLength[3] = remoteState.drumMask3.length;
//   stateUI.partLength[4] = remoteState.drumMask4.length;
//   stateUI.partLength[5] = remoteState.drumMask5.length;
//   for (let i = 0; i < stateUI.drumPartsCount; i++) {
//     setDrumPartLength(i);
//     $id("lengthInput-" + i).value = stateUI.partLength[i];
//   }
//   // SET PART BITMASK
//   stateUI.partBitMask[0] = remoteState.drumMask0;
//   stateUI.partBitMask[1] = remoteState.drumMask1;
//   stateUI.partBitMask[2] = remoteState.drumMask2;
//   stateUI.partBitMask[3] = remoteState.drumMask3;
//   stateUI.partBitMask[4] = remoteState.drumMask4;
//   stateUI.partBitMask[5] = remoteState.drumMask5;
//   for (let i = 0; i < stateUI.drumPartsCount; i++) {
//     setDrumPartBitmask(i);
//   }
//   //  SET CHORD ROOT
//   stateUI.chordRoot = remoteState.chordRoot;
//   chordRootNoteSymbol = chordRootNoteSymbolNormal[stateUI.chordRoot % 12];
//   chordOctCurr = Math.floor((stateUI.chordRoot - 21) / 12);
//   $id("root-note-display").innerHTML = chordRootNoteSymbol;
//   $id("root-oct-display").innerHTML = chordOctCurr;
//   // stateUI.chordRoot = parseInt(chordRootNoteTmp) + 21 + (12 * chordOctCurr);
//   //  SET CHORD GRID
//   stateUI.chordGrid = remoteState.chordGrid;
//   $id("chordGrid").value = stateUI.chordGrid;
//   $id("chordGridLabel").innerHTML = chordGridSymbol[$id("chordGrid").value]; // SET CHORD STEPS NUMBER
//   stateUI.chordStepNumber = remoteState.chordStepNumber;
//   // SET CHORD STEP COUNT
//   stateUI.chordStepNumber = parseInt(remoteState.chordStepNumber);
//   $id("chordStepNumberLabel").innerHTML = stateUI.chordStepNumber;
//   $id("chordStepNumber").value = stateUI.chordStepNumber;
//   setChordPartLength(stateUI.chordStepNumber);
//   // SET CHORD STEPS
//   stateUI.chordStep[0] = remoteState.chordStep0;
//   stateUI.chordStep[1] = remoteState.chordStep1;
//   stateUI.chordStep[2] = remoteState.chordStep2;
//   stateUI.chordStep[3] = remoteState.chordStep3;
//   stateUI.chordStep[4] = remoteState.chordStep4;
//   stateUI.chordStep[5] = remoteState.chordStep5;
//   stateUI.chordStep[6] = remoteState.chordStep6;
//   stateUI.chordStep[7] = remoteState.chordStep7;
//   stateUI.chordStep[8] = remoteState.chordStep8;
//   stateUI.chordStep[9] = remoteState.chordStep9;
//   stateUI.chordStep[10] = remoteState.chordStep10;
//   stateUI.chordStep[11] = remoteState.chordStep11;
//   stateUI.chordStep[12] = remoteState.chordStep12;
//   stateUI.chordStep[13] = remoteState.chordStep13;
//   stateUI.chordStep[14] = remoteState.chordStep14;
//   stateUI.chordStep[15] = remoteState.chordStep15;
//   setChordSteps();
// }
