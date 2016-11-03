#include "num2words.h"
#include "string.h"

static const char* const ONES[] = {
  "zero\n",
  "one\n",
  "two\n",
  "three\n",
  "four\n",
  "five\n",
  "six\n",
  "seven\n",
  "eight\n",
  "nine\n"
};

static const char* const TEENS[] ={
  "\n",
  "eleven\n",
  "twelve\n",
  "thirteen\n",
  "fourteen\n",
  "fifteen\n",
  "sixteen\n",
  "seventeen\n",
  "eighteen\n",
  "nineteen\n"
};

static const char* const TENS[] = {
  "\n",
  "ten\n",
  "twenty\n",
  "thirty\n",
  "forty\n",
  "fifty\n",
  "sixty\n",
  "seventy\n",
  "eighty\n",
  "ninety\n"
};

static const char* STR_OH_CLOCK = "o'clock\n";
static const char* STR_NOON = "\nnoon\n";
static const char* STR_MIDNIGHT = "\nmidnight\n";
static const char* STR_QUARTER = "quarter\n";
static const char* STR_TWELVE = "twelve\n";
static const char* STR_TO = "to\n";
static const char* STR_TWENTY_FIVE_PAST = "twenty\nfive past\n";
static const char* STR_TWENTY_FIVE_TO = "twenty\nfive to\n";
static const char* STR_PAST = "past\n";
static const char* STR_HALF = "half\n";

static size_t append_number(char* words, int num) {
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;

  size_t len = 0;

  if (tens_val > 0) {
    if (tens_val == 1 && num != 10) {
      strcat(words, TEENS[ones_val]);
      return strlen(TEENS[ones_val]);
    }
    strcat(words, TENS[tens_val]);
    len += strlen(TENS[tens_val]);
    if (ones_val > 0) {
      strcat(words, " ");
      len += 1;
    }
  }

  if (ones_val > 0 || num == 0) {
    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  return len;
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);

  size_t written = strlen(str);
  return (length > written) ? written : length;
}

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length) {
  int fuzzy_hours = hours;
  int fuzzy_minutes = ((minutes + 2) / 5) * 5;

  // Handle hour & minute roll-over.
  if (fuzzy_minutes > 55) {
    fuzzy_minutes = 0;
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }

  size_t remaining = length;
  memset(words, 0, length);

  if (fuzzy_minutes != 0 && (fuzzy_minutes >= 10 || fuzzy_minutes == 5 || fuzzy_hours == 0 || fuzzy_hours == 12)) {
    if (fuzzy_minutes == 15) {
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, STR_PAST);
    } else if (fuzzy_minutes == 45) {
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, STR_TO);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 30) {
      remaining -= append_string(words, remaining, STR_HALF);
      remaining -= append_string(words, remaining, STR_PAST);
    } else if (fuzzy_minutes == 25) {
      remaining -= append_string(words, remaining, STR_TWENTY_FIVE_PAST);
    } else if (fuzzy_minutes == 35) {
      remaining -= append_string(words, remaining, STR_TWENTY_FIVE_TO);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes < 30) {
      remaining -= append_number(words, fuzzy_minutes);
      remaining -= append_string(words, remaining, STR_PAST);
    } else {
      remaining -= append_number(words, 60 - fuzzy_minutes);
      remaining -= append_string(words, remaining, STR_TO);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    }
  }

  if (fuzzy_hours == 0) {
    if (fuzzy_minutes == 0) {
      remaining -= append_string(words, remaining, STR_MIDNIGHT);
    } else {
      remaining -= append_string(words, remaining, STR_TWELVE);
    }
  } else if (fuzzy_hours == 12) {
    if (fuzzy_minutes == 0) {
      remaining -= append_string(words, remaining, STR_NOON);
    } else {
      remaining -= append_string(words, remaining, STR_TWELVE);
    }
  } else {
    remaining -= append_number(words, fuzzy_hours % 12);
  }

  if (fuzzy_minutes == 0 && !(fuzzy_hours == 0 || fuzzy_hours == 12)) {
    remaining -= append_string(words, remaining, STR_OH_CLOCK);
  }
}
