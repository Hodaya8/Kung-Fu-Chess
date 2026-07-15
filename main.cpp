#include "io/board_parser.hpp"
#include "io/board_printer.hpp"

#include "input/Controller.hpp"
#include "input/BoardMapper.hpp"
#include "engine/GameEngine.hpp"

#include <iostream>
#include <string>


int main()
{
    try
    {
        Board board = BoardParser::parse(std::cin);


        BoardMapper mapper(
            board.rows(),
            board.cols()
        );


        GameEngine engine(board);


        Controller controller(
            board,
            mapper,
            engine
        );


        std::string command;


        while(std::getline(std::cin, command))
        {

            if(command.rfind("click", 0) == 0)
            {
                int x;
                int y;

                sscanf(
                    command.c_str(),
                    "click %d %d",
                    &x,
                    &y
                );

                controller.click(x,y);
            }
            else if(command.rfind("jump", 0) == 0) // תמיכה בפקודת ה-Jump 
            {
                int x;
                int y;

                sscanf(
                    command.c_str(),
                    "jump %d %d",
                    &x,
                    &y
                );

                controller.jump(x, y);
            }
            else if(command.rfind("wait", 0) == 0)
            {
                int ms;

                sscanf(
                    command.c_str(),
                    "wait %d",
                    &ms
                );

                engine.wait(ms);
            }


            else if(command == "print board")
            {
                BoardPrinter::print(
                    board,
                    std::cout
                );
            }
        }
    }


    catch(const std::exception& e)
    {
        std::cout
            << "ERROR "
            << e.what()
            << std::endl;
    }


    return 0;
}