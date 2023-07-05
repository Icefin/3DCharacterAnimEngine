#include "CharacterLoader.h"

Character* character;

void    initCharacter()
{
    CharacterLoader characterLoader;

    std::string asfFile = "./07-walk.asf";
    std::string amcFile = "./07_05-walk.amc";
  
    character = characterLoader.loadCharacter(asfFile, amcFile);
}

int main()
{
    initCharacter();
}