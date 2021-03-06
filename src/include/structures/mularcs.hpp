#ifndef MULARCS_HPP
#define MULARCS_HPP

namespace structure { 

template<class mcolor_t>
struct Mularcs { 
  using map_t = std::multimap<vertex_t, mcolor_t>; 
  using citer = typename map_t::const_iterator; 
  using criter = typename map_t::const_reverse_iterator;
  using iter = typename map_t::iterator;    

  inline void insert(vertex_t const & v, mcolor_t const & mc) { 
    m_mularcs.insert(std::make_pair(v, mc));	
  } 

  inline void insert(vertex_t const & v, size_t i) {
    if (m_mularcs.find(v) != m_mularcs.end()) { 
      m_mularcs.find(v)->second.insert(i);
    } else { 
      m_mularcs.insert(std::make_pair(v, mcolor_t(i)));
    }
  }
 
  inline void erase(vertex_t const & v) { 
    m_mularcs.erase(v);
  }
 
  inline void erase(vertex_t const & v, mcolor_t const & color) { 
    for (auto iter = m_mularcs.begin(); iter != m_mularcs.end();) { 
      if (iter->first == v && iter->second == color) { 
        m_mularcs.erase(iter++);    
      } else { 
        ++iter;
      }
    }    
  }

  inline vertex_t get_vertex(mcolor_t const & color) const { 
    vertex_t v = "";

    for (auto const & arc : m_mularcs) {
      if (arc.second == color) { 
        v = arc.first;
        break;
      }
    }

    return v;   
  } 
  
  inline std::pair<citer, citer> equal_range(vertex_t const & v) const {
    return m_mularcs.equal_range(v);
  } 
 
  inline mcolor_t union_multicolors() const {
    mcolor_t union_color; 
    
    for (auto const & arc : m_mularcs) { 
      mcolor_t temp(union_color, arc.second, mcolor_t::Union);  
      union_color = temp;
    }

    return union_color;
  }
 
  inline bool defined(vertex_t const & v) const { 
    return (m_mularcs.count(v) != 0);
  }
 
  inline size_t size() const {
    return m_mularcs.size();  
  }

  DECLARE_ITERATOR( iter, m_mularcs, begin, begin )
  DECLARE_ITERATOR( iter, m_mularcs, end, end )
  DECLARE_CONST_ITERATOR( citer, m_mularcs, begin, cbegin )
  DECLARE_CONST_ITERATOR( citer, m_mularcs, end, cend )
  DECLARE_CONST_ITERATOR( citer, m_mularcs, cbegin, cbegin )
  DECLARE_CONST_ITERATOR( citer, m_mularcs, cend, cend )
  DECLARE_CONST_ITERATOR( criter, m_mularcs, crbegin, crbegin )
  DECLARE_CONST_ITERATOR( criter, m_mularcs, crend, crend )

private: 
  map_t m_mularcs;
}; 

}

#endif
