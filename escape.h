#include <iostream>
#include <string>
#include <sstream>
#include <sys/ioctl.h>

/*
    ___Console text styles list__
   |id  -  act  param   value    |
   |_____________________________|
   |0   - unset [all]     -      |
   |1   -   set style   bold     |
   |4   -   set style   underline|
   |7   -   set style   marker   |
   |22  - unset style   bold     |
   |24  - unset style   underline|
   |27  - unset style   marker   |
   |30  -   set f_color white    |
   |31  -   set f_color red      |
   |32  -   set f_color green    |
   |33  -   set f_color gold     |
   |34  -   set f_color blue     |
   |35  -   set f_color magenta  |
   |36  -   set f_color cyan     |
   |37  -   set f_color dark     |
   |39  - unset f_color   -      |
   |40  -   set b_color white    |  
   |41  -   set b_color red      |
   |42  -   set b_color green    |
   |43  -   set b_color gold     |
   |44  -   set b_color blue     |
   |45  -   set b_color magenta  |
   |46  -   set b_color cyan     |
   |47  -   set b_color dark     |
   |49  - unset b_color   -      |
   |90  -   set f_color black    |
   |91  -   set f_color rose     |
   |92  -   set f_color olive    |
   |93  -   set f_color yellow   |
   |94  -   set f_color sky      |
   |95  -   set f_color pink     |
   |96  -   set f_color waves    |
   |97  -   set f_color white    |
   |100 -   set b_color black    |
   |101 -   set b_color rose     |
   |102 -   set b_color olive    |
   |103 -   set b_color yellow   |
   |104 -   set b_color sky      |
   |105 -   set b_color pink     |
   |106 -   set b_color waves    |
   |107 -   set b_color white    |
   |_____________________________|
*/
namespace console
    {
    int col ()
        {
        struct winsize w;
        ioctl (0, TIOCGWINSZ, &w);
        return (w.ws_col);
        }
    int row ()
        {
        struct winsize w;
        ioctl (0, TIOCGWINSZ, &w);
        return (w.ws_row);
        }
    
    std::ostream& ED (std::ostream& s)
        {
        return  s << std::string ("\033[2J");
        }
    
    std::ostream& EL (std::ostream& s)
        {
        return  s << std::string ("\033[K");
        }
    
    class estream
        {
        private:
        std::string escape;
        public:
        estream(std::string e) : escape (e) {};
        friend std::ostream& operator<< (std::ostream&, estream);
        
        };
    
    std::ostream &operator<< (std::ostream &s, estream e)
        {
        s << e.escape << std::flush;
        return s;
        }
    
    estream setpos (int y, int x)
        {
        std::ostringstream sout;
        sout << "\033[" << y << ";" << x << "H";
        return estream (sout.str ());
        }
    
    estream SGR (int r)
        {
        std::ostringstream sout;
        sout << "\033[" << r << "m";
        return estream (sout.str ());
        }
    }
