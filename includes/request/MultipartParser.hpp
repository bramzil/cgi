# ifndef MULTIPARTPARSER_HPP
# define MULTIPARTPARSER_HPP

# include "../extern.hpp"
# include "../utilities.hpp"

class MultipartParser {

    public:

        typedef void        (MultipartParser::*memberPtr)( void );

        void                removeBoundaryWord( void );
        void                extractPartHeaders( void );
        void                parsePartHeaders( void );
        void                parseSingleHeaderFiled(std::string& headerField);
        void                openPartDestinationFile( void );
        void                extractPartContent();

        void                operator()( void );
        bool                isDone( void );

        MultipartParser(std::string& boundaryArg, std::string request_body);
        ~MultipartParser( void );

    private:
        bool                                parsingDone;
        std::string                         boundaryWord;
        std::string                         multipartBuffer;
        std::string                         partHeaders;
        int                                 requestBodyFd;
        memberPtr                           currentFunction;
        std::fstream                        currentPartStream;
        std::map<std::string, std::string>  currentPartHeaders;
};

# endif