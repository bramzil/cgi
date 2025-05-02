# ifndef RQUESTPROCESSOR_HPP
# define RQUESTPROCESSOR_HPP

# include "Request.hpp"
# include "MultipartParser.hpp"
# include "../extern.hpp"
# include "../utilities.hpp"
# include "../core/Server.hpp"
# include "../core/Connection.hpp"
# include "../core/Socket.hpp"
# include "../core/Location.hpp"
# include "../request/CgiModule.hpp"

class RequestProcessor: public Request {
    public:
        typedef void                        (RequestProcessor::*mbrPtr)( void );

        void                                startProcessing( void );

        void                                processGetRequest( void );
        void                                generateDirectoryHTMLIndexing( void );
        void                                generateDirectoryHTMLContent(DIR*  directory);
        
        void                                processPostRequest( void );
        void                                readRequestBody( void );
        void                                getChunkSize( void );
        void                                extractChunk( void );
        void                                removeChunkLimiter( void );
        void                                parceRequestBody( void );
        void                                multipartMonitor( void );
        
        void                                processDeleteRequest( void );
        void                                deleteFile(const std::string& filePath);
        void                                deleteDirectory(const std::string& dirPath);
        void                                deleteResource(const std::string& path);
        std::string                         getCGIExecutablePath( void );
        std::string                         extractScriptName( void );
        void                                cgiInstantiator( void );
        void                                cgiMonitor( void );

        std::string                         variablesExpander(const std::string& str) const;
        std::string                         getDefaultCookies(const std::set<std::vector<std::string> >& defaultCookies) const;
        
        void                                operator() (Connection* connection);
        bool                                isDone( void );

        RequestProcessor(Request& request);
        RequestProcessor(const RequestProcessor& x);
        virtual ~RequestProcessor( void );

    private:
        bool                                processingDone;
        size_t                              chunkSize;
        std::string                         chunkBuffer;

        size_t                              contentLength;
        std::string                         requestBodyFile;
        std::fstream                        requestBodyStream;

        mbrPtr                              currentFunction;
        MultipartParser                     *multipartInstance;
        CgiModule                           *cgiApplicationInstance;
};

# endif