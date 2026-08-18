// Copyright Vladimir Prus 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_HPP_VP_2004_08_24
#define BOOST_DLL_HPP_VP_2004_08_24

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#ifdef _WIN32
# include <windows.h>
#else
# include <dlfcn.h>
#endif

#include <iostream>

namespace utilmm { namespace plugin {

    struct killer 
    {
        killer(void* h) : h(h) {}
        template<class T>
        void operator()(T)
        {
            std::cout << "Killing DLL\n";
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(h));
#else
            dlclose(h);
#endif
        }
        void* h;
    };

    class dll {
    public:
        dll() {} // TODO: should remove this or make non-public
        dll(const std::string& name) : m_name(name) {}

        template<typename SymbolType>
        boost::shared_ptr<
            typename boost::remove_pointer<SymbolType>::type> 
        get(const std::string& symbol_name) const
        {
            // TODO: static assert that SymbolType is a pointer.
            typedef typename boost::remove_pointer<SymbolType>::type PointedType;
            
            // Open the library. Yes, we do it on every access to 
            // a symbol, see the design discussion in the documentation.
#ifdef _WIN32
            void* handle = LoadLibraryA(m_name.c_str());
#else
            void* handle = dlopen(m_name.c_str(), RTLD_LAZY|RTLD_GLOBAL);
#endif
            if (!handle) {
                throw std::logic_error("Could not open DLL");
            }
#ifdef _WIN32
            void* address = reinterpret_cast<void*>(
                GetProcAddress(static_cast<HMODULE>(handle), symbol_name.c_str()));
            if (!address) {
                FreeLibrary(static_cast<HMODULE>(handle));
                throw std::logic_error("Could not resolve symbol in DLL");
            }
#else
            // Clear the error state.
            dlerror();
            void* address = dlsym(handle, symbol_name.c_str());
            char* error = dlerror();
            if (error) {
                throw std::logic_error(error);
            }
#endif
            // Cast the to right type.
            SymbolType s = (SymbolType)(address);

            boost::shared_ptr<PointedType> result(s, killer(handle));
            
            return result;            
        }        

    private:
        std::string m_name;
    };

}}

#endif
