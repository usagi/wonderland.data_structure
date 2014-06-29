#include <chrono>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include <mutex>

#include <thread>

#ifdef EMSCRIPTEN
  #include <sstream>
  #include <emscripten/emscripten.h>
#endif

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace data_structure
    {
      
      template
      < class T_element = std::string
      , class T_clock   = std::chrono::steady_clock
      >
      class time_deferred_buffer_t
      {
      public:
        using type              = time_deferred_buffer_t< T_element, T_clock >;
        using clock_t           = T_clock;
        using time_point_t      = typename clock_t::time_point;
        using element_t         = std::string;
        using data_t            = std::map< time_point_t, element_t >;
        using time_function_t   = std::function< auto ( ) -> time_point_t >;
        using flush_parameter_t = typename data_t::const_iterator;
        using flush_t           = std::function< auto ( const flush_parameter_t&, const flush_parameter_t& ) -> void >;
        using duration_t        = typename time_point_t::duration;
        
      private:
        data_t          _data;
        time_function_t _now;
        flush_t         _flush;
        duration_t      _delay;
        
        std::function< auto ( const element_t&  value ) -> void > _push_l;
        std::function< auto (       element_t&& value ) -> void > _push_r;
        
        std::mutex  _data_mutex;
        
#ifndef EMSCRIPTEN
        std::thread _auto_update_thread;
        bool        _auto_update_thread_continue;
#endif
        
        time_point_t _last_update_time;
        
        auto update_impl( )
          -> void
        {
          const auto n = _now( );
          const auto upper_bound_time = n - _delay;
          
          // it is equals to `if ( n - _last_update_time < _delay )`
          if ( upper_bound_time < _last_update_time )
            return;
          
          _last_update_time = n;
          
          const auto upper_bound = _data.upper_bound( upper_bound_time );
          _flush( _data.cbegin( ), upper_bound );
          _data.erase( _data.cbegin( ), upper_bound );
        }
        
      public:
        
        time_deferred_buffer_t( )
          : _now( clock_t::now )
          , _flush( [ ]( const flush_parameter_t&, const flush_parameter_t& ){ } )
          , _delay( 0 )
#ifndef EMSCRIPTEN
          , _auto_update_thread_continue( false )
#endif
        {
          push_with_update( true );
        }
        
        ~time_deferred_buffer_t( )
        {
#ifndef EMSCRIPTEN
          auto_update( false );
#endif
          force_flush( );
        }
        
        auto now( const time_function_t& f )
          -> void
        { _now = f; }
        
        auto flush( const flush_t& f )
          -> void
        { _flush = f; }
        
        auto clear( )
          -> void
        { _data.clear(); }
        
        auto force_flush()
          -> void
        {
          _flush( _data.cbegin(), _data.cend() );
          _data.clear();
        }
        
        auto delay( const duration_t& d )
          -> void
        {
          _delay = d;
        }
        
        auto delay()
          -> const duration_t&
        {
          return _delay;
        }
        
        auto push_with_update( const bool enable )
          -> void
        {
          if ( enable )
          {
            _push_l = [&]( const element_t& value ){ _data.emplace( _now( ), value ); this -> update_impl( ); };
            _push_r = [&]( element_t&& value ){ _data.emplace( _now( ), std::move( value ) ); this -> update_impl( ); };
          }
          else
          {
            _push_l = [&]( const element_t& value ){ _data.emplace( _now( ), value ); };
            _push_r = [&]( element_t&& value ){ _data.emplace( _now( ), std::move( value ) ); };
          }
        }
        
        auto push( const element_t& value )
          -> void
        {
          std::lock_guard< std::mutex > l( _data_mutex );
          _push_l( value );
        }
        
        auto push( element_t&& value )
          -> void
        {
          std::lock_guard< std::mutex > l( _data_mutex );
          _push_r( std::move(value) );
        }
        
        auto update( )
          -> void
        {
          std::lock_guard< std::mutex > l( _data_mutex );
          update_impl();
        }
        
        auto auto_update( bool enable = true )
          -> void
        {
#ifdef EMSCRIPTEN
          std::cerr << "`time_deferred_buffer_t::auto_update` is not support for Emscripten yet.\n";
#else
          if ( enable )
          {
            if ( _auto_update_thread.joinable() )
              return;
            
            _auto_update_thread_continue = true;
            
            _auto_update_thread = std::thread
            ( [ this ]
              {
                while ( this -> _auto_update_thread_continue )
                {
                  this -> update();
                  std::this_thread::sleep_for( this -> delay() );
                }
              }
            );
          }
          else
          {
            if ( _auto_update_thread.joinable() )
            {
              _auto_update_thread_continue = false;
              _auto_update_thread.join();
            }
          }
#endif
        }
      };
      
    }
  }
}