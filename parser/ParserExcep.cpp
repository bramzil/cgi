# include "../includes/parser/ParserExcep.hpp"

const char* ParserExcep::what( void ) const throw() {
    return (buffer.c_str());
}

ParserExcep::ParserExcep(std::string trigger, std::string description, size_t& l_nbr) {
    buffer += trigger;
    buffer += ": ";
    buffer += description;
    buffer += " at line: ";
    buffer += size_t_to_string(l_nbr);
}

ParserExcep::ParserExcep(const ParserExcep& ex) {
    *this = ex;
}

ParserExcep&    ParserExcep::operator=(const ParserExcep& ex) {
    buffer = ex.buffer;
    return (*this);
}

ParserExcep::~ParserExcep() throw() {
    // nothing to do for this time.
}