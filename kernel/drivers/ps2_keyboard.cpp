#include <drivers/ps2.h>

using namespace drivers;

enum class StrokeType
{
    Pressed,
    Released
};

enum class SpecialCharacterType
{
    Normal,
    Escape,
    Backspace,
    Tab,
    Enter,
    LeftControl,
    LeftShift,
    RightShift,
    LeftAlt,
    CapsLock,
    F_Key,
};

constexpr inline struct 
{
    uint8_t                 value;
    const char*             explanation;
    StrokeType              type;
    SpecialCharacterType    special_type = SpecialCharacterType::Normal;

} keys[] = {
    {'`', "Excape Pressed", StrokeType::Pressed, SpecialCharacterType::Escape},
    {'1', "1 Pressed", StrokeType::Pressed},
    {'2', "2 Pressed", StrokeType::Pressed},
    {'3', "3 Pressed", StrokeType::Pressed},
    {'4', "4 Pressed", StrokeType::Pressed},
    {'5', "5 Pressed", StrokeType::Pressed},
    {'6', "6 Pressed", StrokeType::Pressed},
    {'7', "7 Pressed", StrokeType::Pressed},
    {'8', "8 Pressed", StrokeType::Pressed},
    {'9', "9 Pressed", StrokeType::Pressed},
    {'0', "0 Pressed", StrokeType::Pressed},
    {'-', "- Pressed", StrokeType::Pressed},
    {'+', "+ Pressed", StrokeType::Pressed},
    {'\0', "Backspace Pressed", StrokeType::Pressed, SpecialCharacterType::Backspace},
    {'\0', "Tab Pressed", StrokeType::Pressed, SpecialCharacterType::Tab},
    {'Q', "Q Pressed", StrokeType::Pressed},
    {'W', "W Pressed", StrokeType::Pressed},
    {'E', "E Pressed", StrokeType::Pressed},
    {'R', "R Pressed", StrokeType::Pressed},
    {'T', "T Pressed", StrokeType::Pressed},
    {'Y', "Y Pressed", StrokeType::Pressed},
    {'U', "U Pressed", StrokeType::Pressed},
    {'I', "I Pressed", StrokeType::Pressed},
    {'O', "O Pressed", StrokeType::Pressed},
    {'P', "P Pressed", StrokeType::Pressed},
    {'[', "[ Pressed", StrokeType::Pressed},
    {']', "] Pressed", StrokeType::Pressed},
    {'\0', "Enter Pressed", StrokeType::Pressed, SpecialCharacterType::Enter},
    {'\0', "Left Control Pressed", StrokeType::Pressed, SpecialCharacterType::LeftControl},
    {'A', "A Pressed", StrokeType::Pressed},
    {'S', "S Pressed", StrokeType::Pressed},
    {'D', "D Pressed", StrokeType::Pressed},    
    {'F', "F Pressed", StrokeType::Pressed},
    {'G', "G Pressed", StrokeType::Pressed},
    {'H', "H Pressed", StrokeType::Pressed},
    {'J', "J Pressed", StrokeType::Pressed},
    {'K', "K Pressed", StrokeType::Pressed},
    {'L', "L Pressed", StrokeType::Pressed},
    {';', "; Pressed", StrokeType::Pressed},
    {'\'', "\' Pressed", StrokeType::Pressed},
    {'`', "` Pressed", StrokeType::Pressed},
    {'\0', "Left Shift Pressed", StrokeType::Pressed, SpecialCharacterType::LeftShift},
    {'\\', "\\ Pressed", StrokeType::Pressed},
    {'Z', "Z Pressed", StrokeType::Pressed},
    {'X', "X Pressed", StrokeType::Pressed},
    {'C', "C Pressed", StrokeType::Pressed},
    {'V', "V Pressed", StrokeType::Pressed},
    {'B', "B Pressed", StrokeType::Pressed},
    {'N', "N Pressed", StrokeType::Pressed},
    {'M', "M Pressed", StrokeType::Pressed},
    {',', ", Pressed", StrokeType::Pressed},
    {'.', ". Pressed", StrokeType::Pressed},
    {'/', "/ Pressed", StrokeType::Pressed},
    {'\0', "Right Shift Pressed", StrokeType::Pressed, SpecialCharacterType::RightShift},
    {'*', "* Pressed", StrokeType::Pressed},
    {'\0', "Left Alt Pressed", StrokeType::Pressed, SpecialCharacterType::LeftAlt},
    {' ', "Space Pressed", StrokeType::Pressed},
    {'\0', "Caps Lock Pressed", StrokeType::Pressed, SpecialCharacterType::CapsLock},
    {'1', "F1 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'2', "F2 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'3', "F3 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'4', "F4 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'5', "F5 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'6', "F6 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'7', "F7 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'8', "F8 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'9', "F9 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'0', "F10 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
    {'-', "F11 Pressed", StrokeType::Pressed, SpecialCharacterType::F_Key},
}
;

kernel::keyboard::InputKeyType ps2::keyboard::KeyboardSource(void* args)
{
    auto input = inb(ps2::DATA_PORT);

    auto key = keys[input - 1]; // indexing from 1

#if defined(K_LOG_KEY_STROKES)
    LOG_SA("PS/2-KEYBOARD: ", "Recieved key: %c (%s)\n", key.value, key.explanation);
#endif 

    return {key.value};
}

