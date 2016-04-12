#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <termios.h>
#include <unistd.h>
#include "test.h"

/**
 * Simple proof of concept - currently just gets user email/password and
 * returns their token (as provided by Discord) which will be needed later
 * when making any other request of the Discord API 
 */
int main(int argc, char * argv[]) {
  
  CURL *curl;
  CURLcode err;

  // get uname and pw

  std::cout << "Please enter your email: ";
  std::string email = "";
  std::string pass = "";
  getline(std::cin, email);
  std::cout << "Please enter your password: ";
  struct termios save, modify;

   tcgetattr (STDIN_FILENO, &save);
   modify = save;
   modify.c_lflag &= ~ECHO;
   if (tcsetattr(STDIN_FILENO, TCSANOW, &modify) != 0)
     return -1;
   getline(std::cin, pass);
   if (tcsetattr(STDIN_FILENO, TCSANOW, &save) != 0) 
     return -1;


  // create CURL handle
  curl = curl_easy_init();


  if (curl) {

    std::string JSONresult;
    std::string post = "email=" + email + "&password=" + pass;

    // Discord auth URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://discordapp.com/api/auth/login");
    // uname and pw
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str()); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &JSONresult);

    Json::Reader reader;
    Json::Value result;

    err = curl_easy_perform(curl);

    bool successfulParse = reader.parse(JSONresult, result);

    if (!successfulParse) {
      std::cout << "Parsing error: " << reader.getFormattedErrorMessages();
      exit(0);
    }

    if (err != CURLE_OK) {
      std::cout << "Error: " << curl_easy_strerror(err);
      exit(0);
    }

    std::string token;

    if (result.isMember("token")) {
      token = result.get("token", result).asString();
      std::cout << "\nResult: " << token << "\n";
    } else {
      std::cout << "\nError: No token received, check your email and password!\n";
      return 0;
    }

    curl_easy_cleanup(curl);

  }

  return 0;
}


size_t writeString(void * contents, size_t size, size_t nmemb, void * userp) {
  ((std::string *)userp)->append((char *) contents, size * nmemb);
  return size * nmemb;
}
