#include <map>
#include <string>
#include <libudev.h>

using namespace std;
typedef map<string, int> device;

class UdevManager
{
  public:
    UdevManager();
    ~UdevManager();

    void setup();
    void readEvent();

  private:
    device keyboards;
    device mice;

    struct udev *udev;
    struct udev_monitor *mon;
    int udev_fd;

    void addKeyboard(const char *node);
    void removeKeyboard(const char *node);
    void addMouse(const char *node);
    void removeMouse(const char *node);
    void printDevice();

    void setupUdev();
    void setupKeyboard();
    void setupMouse();

    void readUdevEvent();
    void readKeyboardEvent();
    void readMouseEvent();
};
