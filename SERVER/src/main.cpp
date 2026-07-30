#include "application/server_application.hpp"
#include <iostream>
#include <exception>

int main()
{
    try
    {
        // הפורט שעליו שרת ה-C++ שלנו ירוץ
        const int port = 9000;

        std::cout << "[MAIN] Starting KungFu Chess Server on port " << port << "..." << std::endl;

        // יצירת השרת - עכשיו מקבל רק את הפורט, בלי ה-Database!
        KungFuChessServerApplication app(port);
        
        // הרצת השרת
        return app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[MAIN FATAL] Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "[MAIN FATAL] Unknown error occurred." << std::endl;
        return 1;
    }
}