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
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "fg_color",
        "defaultValue": "0xFFFF55",
        "label": "Foreground  Color"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
