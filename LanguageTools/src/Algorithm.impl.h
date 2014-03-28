// helper functions for containers (uses c++ name conventions)
// ------------------------------------------------------------------//

#include <algorithm>
#include <sstream>
#include <cassert>

namespace lt
{
#if __cplusplus < 201103L // c++11 has std::copy_if
    // copy a container filtering on predicate
    template <typename InputIterator, typename OutputIterator, typename Predicate>
    OutputIterator copy_if(InputIterator first, InputIterator last, OutputIterator dest, Predicate p)
    {
        for(;first != last; ++first)
        {
            if(p(*first))
            {
                *dest = *first;
                ++dest;
            }
        }
        return dest;
    }
#endif

    template <typename T>
    std::string ArrayString(const std::vector<T>& v)
    {
        std::stringstream os;
        os << "{";
        for (size_t i = 0; i < v.size(); ++i)
        {
            os << v.at(i);
            if (i != (v.size() - 1)) os << ", ";
        }
        os << "}";
        return os.str();
    }


    template <typename T>
    std::string ArrayString(T const * const array, const int N)
    {
        // test boundary conditions
        assert(!(array == NULL && N > 0));
    
        std::stringstream os;
        os << "{";
        for (int i = 0; i < N; ++i)
        {
            os << array[i];
            if (i != (N - 1)) os << ", ";
        }
        os << "}";
        return os.str();
    }

    // filter the container
    template <typename Container, typename Predicate>
    Container filter_container(const Container &c, Predicate p)
    {
        Container result;
#if __cplusplus < 201103L // c++11 has std::copy_if
        lt::copy_if(c.begin(), c.end(), std::back_inserter(result), p);
#else
        std::copy_if(c.begin(), c.end(), std::back_inserter(result), p);
#endif
        return result;
    }

    // combine containers 
    template <typename Container>
    Container combine_container(const Container &c1, const Container &c2)
    {
        Container result(c1);
        result.insert(c1.end(), c2.begin(), c2.end());
        return result;
    }

    // delete a pointer 
    template <typename T>
    struct delete_ptr : public std::unary_function<bool,T>
    {
       bool operator()(T* ptr) const {delete ptr; return true;}
       bool operator()(T ptr ) const {delete ptr; return true;}
    };

    // delete the container
    template <typename Container>
    void delete_container(const Container &c)
    {
        std::for_each(c.begin(), c.end(), delete_ptr<typename Container::value_type>());
    }

    // initialze container
    template <typename Container, typename Type>
    void init_container(Container &c, const Type& t)
    {
        std::for_each(c.begin(), c.end(), t);
    }

} // namespace lt
