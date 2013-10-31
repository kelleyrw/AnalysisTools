#ifndef LT_ALGORITHM_H
#define LT_ALGORITHM_H

// helper functions for containers (uses c++ name conventions)
// ------------------------------------------------------------------//

namespace lt
{
    // filter the container
    template <typename Container, typename Predicate>
    Container filter_container(const Container &c, Predicate p);

    // combine containers 
    template <typename Container>
    Container combine_container(const Container &c1, const Container &c2);

    // delete the container
    template <typename Container>
    void delete_container(const Container &c);

    // initialize container
    template <typename Container, typename Type>
    void init_container(const Container &c, const Type& t = Type());

} // namespace lt

// templated function definitions
#include "AnalysisTools/LanguageTools/src/Algorithm.impl.h"

#endif // LT_ALGORITHM_H
