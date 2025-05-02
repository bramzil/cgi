# ifndef PARSEREXCEP_HPP
# define PARSEREXCEP_HPP

# include "../extern.hpp"
# include "../utilities.hpp"

class ParserExcep : public std::exception
{
    public:

        const char*                     what( void ) const throw();

        ParserExcep(std::string trigger, std::string description, size_t& l_nbr);
        ParserExcep(const ParserExcep& ex);
        ParserExcep&                    operator=(const ParserExcep& ex);
        virtual ~ParserExcep() throw();

    private:
        std::string     buffer;

};

# endif