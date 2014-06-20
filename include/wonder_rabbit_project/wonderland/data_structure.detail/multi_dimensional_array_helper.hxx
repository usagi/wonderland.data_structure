#pragma once

#include <array>
#include <cstddef>

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace data_structure
    {
      template
      < class T_element
      , std::size_t ... T_dimensions
      >
      class multi_dimensional_array_helper final
      {
      private:
        
        template < std::size_t ... T_internal_dimension >
        struct internal_type_helper
        {
          using type = T_element;
        };
        
        template < std::size_t T_internal_dimension, std::size_t ... T_internal_dimensions >
        struct internal_type_helper < T_internal_dimension, T_internal_dimensions ... >
        {
          using type = typename std::array
            < typename internal_type_helper< T_internal_dimensions ... >::type
            , T_internal_dimension
            >;
        };
        
      public:
        
        using type = typename internal_type_helper< T_dimensions ... >::type;
        
      };
      
    }
  }
}