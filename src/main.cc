#include "util/inv_application.h"

#include "dyeing_handler.h"

int main(int argc, char **argv) {
  INIT_INV_LOGGER("INV", "dyeing");
  InvTimeLogger::getInstance()->setSeparator("");
  InvTimeLogger::getInstance()->initFormat("ad", "%Y%m%d%H");

  signal(SIGPIPE, SIG_IGN);

  INVApplication<inv::ABDyeingHandler, inveno::ABDyeingServiceProcessor> app;
  app.start(argc, argv);

  return 0;
}
