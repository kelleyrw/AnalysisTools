// helper functions for containers (uses c++ name conventions)
// ------------------------------------------------------------------//

#include <algorithm>

namespace lt
{
    // filter the container
    template<typename Container, typename Predicate>
    Container filter_container(const Container &c, Predicate p)
    {
        Container result;
        std::copy_if(c.begin(), c.end(), std::back_inserter(result), p);
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

} // namespace lt
