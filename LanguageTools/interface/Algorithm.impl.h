// helper functions for containers (uses c++ name conventions)
// ------------------------------------------------------------------//

#include <algorithm>

namespace lt
{
    // copy a container filtering on predicate
    template< typename InputIterator, typename OutputIterator, typename Predicate >
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

    // filter the container
    template<typename Container, typename Predicate>
    Container filter_container(const Container &c, Predicate p)
    {
        Container result;
        copy_if(c.begin(), c.end(), std::back_inserter(result), p);
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
    template< typename T >
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

    // only works on compile time arrays (e.g. float bins[] = {1,2,3};)
    template <int N> unsigned int find_bin(const float value, const float (&bins)[N])
    {
        int index = 0;
        for (size_t i=0; i != N-1; i++)
        {
            if (bins[i] < value && value < bins[i+1]) 
            {
                index=i;
                break;
            }
        }
        return index;
    }

    template <int N> unsigned int find_bin(const double value, const double (&bins)[N])
    {
        int index = 0;
        for (size_t i=0; i != N-1; i++)
        {
            if (bins[i] < value && value < bins[i+1]) 
            {
                index=i;
                break;
            }
        }
        return index;
    }

} // namespace lt
