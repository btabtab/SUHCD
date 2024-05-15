#include <raylib.h>

class UIElement
{
private:
    int x, y, width, height;
    Color colour;

    bool was_clicked;
    bool is_under_cursor;

public:
    UIElement(int new_x, int new_y, int new_width, int new_height, Color new_colour)
    :
    x(new_x),
    y(new_y),
    width(new_width),
    height(new_height),
    colour(new_colour)
    {
        was_clicked = false;
    }
    void drawUIElemelent()
    {
        if(was_clicked)
        {
        }
        if(is_under_cursor)
        {
        }
    }
    bool wasThisClicked()
    {
    }
    bool wasThisHoveredOver()
    {
    }
};