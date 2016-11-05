module.exports = [
  {
    "type": "heading",
    "defaultValue": "T Time configuration"
  },
  {
    "type": "text",
    "defaultValue": "configure your T Time the way you want it ..."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "watch color settings"
      },
      {
        "type": "color",
        "messageKey": "bg_color",
        "defaultValue": "0x000055",
        "label": "background color",
        "description": "everyday background"
      },
      {
        "type": "color",
        "messageKey": "fg_color",
        "defaultValue": "0xFFFF55",
        "label": "font color",
        "description": "everyday text"
      }
      ]
  },
   {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "date settings"
      },
      {
        "type": "slider",
        "messageKey": "datePeek",
        "defaultValue": 1.5,
        "label": "date peek (seconds)",
        "description": "how long do you want to show the date for?",
        "min": 0,
        "max": 5,
        "step": 0.5
      },
      {
        "type": "color",
        "messageKey": "dt_bg_color",
        "defaultValue": "0x000055",
        "label": "background color",
        "description": "date background"
      },
      {
        "type": "color",
        "messageKey": "dt_fg_color",
        "defaultValue": "0xFFFF55",
        "label": "font color",
        "description": "date text"
      }
    ]
  },
  {
  "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "bluetooth down"
      },
      {
        "type": "text",
        "defaultValue": "where's my phone?"
      },
      {
        "type": "color",
        "messageKey": "bg_invert",
        "defaultValue": "0x55FFFF",
        "label": "background color",
        "description": "bluetooth lost background"
      },
      {
        "type": "color",
        "messageKey": "fg_invert",
        "defaultValue": "0xAA00AA",
        "label": "font color",
        "description": "bluetooth lost font"
      },
      {
        "type": "color",
        "messageKey": "fg_alert",
        "defaultValue": "0xFF0000",
        "label": "bluetooth alert color",
        "description": "bluetooth warning text"
      }
      ]
  },
  {
   "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "charging indicator"
      },
      {
        "type": "color",
        "messageKey": "fg_chg_low",
        "defaultValue": "0xFF0000",
        "label": "low charge color",
        "description": "under 35%"
      },
      {
        "type": "color",
        "messageKey": "fg_chg_med",
        "defaultValue": "0xFFFF55",
        "label": "mid charge color",
        "description": "under 75%"
      },
      {
        "type": "color",
        "messageKey": "fg_chg_full",
        "defaultValue": "0x00FF00",
        "label": "high charge color",
        "description": "nearly full"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "save settings"
  }
];
