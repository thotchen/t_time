module.exports = [
  {
    "type": "heading",
    "defaultValue": "T Time configuration"
  },
  {
    "type": "text",
    "defaultValue": "Configure T Time just the way you like it."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Date settings"
      },
      {
        "type": "slider",
        "messageKey": "datePeek",
        "defaultValue": 1.5,
        "label": "Date peek",
        "description": "Show the date for how long?",
        "min": 0,
        "max": 5,
        "step": 0.5
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Color settings"
      },
      {
        "type": "color",
        "messageKey": "bg_color",
        "defaultValue": "0x000055",
        "label": "Normal Background Color"
      },
      {
        "type": "color",
        "messageKey": "fg_color",
        "defaultValue": "0xFFFF55",
        "label": "Normal Foreground  Color"
      },
      {
        "type": "color",
        "messageKey": "bg_invert",
        "defaultValue": "0x55FFFF",
        "label": "Inverted Background Color"
      },
      {
        "type": "color",
        "messageKey": "fg_invert",
        "defaultValue": "0xAA00AA",
        "label": "Inverted Foreground  Color"
      },
      {
        "type": "color",
        "messageKey": "fg_alert",
        "defaultValue": "0xFF0000",
        "label": "Inverted Alert Color"
      },
      {
        "type": "color",
        "messageKey": "fg_chg_low",
        "defaultValue": "0xFF0000",
        "label": "Low Charge Color"
      },
      {
        "type": "color",
        "messageKey": "fg_chg_med",
        "defaultValue": "0xFFFF55",
        "label": "Mid Charge  Color"
      },
      {
        "type": "color",
        "messageKey": "fg_chg_full",
        "defaultValue": "0x00FF00",
        "label": "High Charge Color"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
