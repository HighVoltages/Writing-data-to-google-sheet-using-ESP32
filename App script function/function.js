function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var data = JSON.parse(e.postData.contents); // Parse incoming JSON

  // Append new data to the Google Sheet
  if (data.method === "append") {
    sheet.appendRow([new Date(),data.timestamp, data.temperature, data.humidity, data.buttonState]);
  } else if (data.method === "replace") {
    sheet.clear(); // Clear the sheet and replace the data
    sheet.appendRow(["Sheet Time","ESP32 Time", "Temperature", "Humidity", "Button State"]);
    sheet.appendRow([new Date(),data.timestamp, data.temperature, data.humidity, data.buttonState]);
  }

  return ContentService.createTextOutput("Data received successfully");
}
