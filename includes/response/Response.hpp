# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../extern.hpp"
# include "../core/Connection.hpp"
# include "../request/Request.hpp"

class Response : public Request {
    public:
    
        typedef void                    (Response::*mbrPtr)( void );
        
        void                            procedRespondProcess( void );
        
        void                            respondWithError( void );

        void                            respondByRedirecting( void );

        void                            respondWithDefaultResponse( void );
        
        void                            generateDefaultPayload( void );
        void                            serveGeneratedContentToClient( void );

        void                            respondWithRegularResource( void );

        void                            respondWithDirectoryIndexing( void );

        void                            respondWithBackEndResponse( void );
        void                            extractBackEndHeaderSection( void );
        void                            extractBackEndHeaders(std::string& headerSection);
        void                            convertBackEndHeadersToHTTPHeaders( void );

        void                            extractAndServeResourceContent( void );
        void                            serveExtractedDataToClient( void );

        void                            sessionManagementCookies( void );
        std::string                     cookieToHeaderFieldValue(std::vector<std::string> cookie);
        
        void                            resetResponseAttribute( void );
        
        
        void                            operator()(Connection* connection);
        bool                            isDone( void );
        void                            initiateStatusCodeList( void );
        std::string                     getStatusPhrase( void );

        Response(const Request& request);
        ~Response();

    protected:

        mbrPtr                              currentFunction;

        bool                                respondingDone;
        std::string                         generatedHeaders;
        
        std::ifstream                       inputFstream;
        size_t                              responseSize;

        std::map<short, std::string>        statusCodeList;
        std::map<std::string, std::string>  backEndHeaders;
};

# endif