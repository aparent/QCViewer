#include <map>
#include <string>
#include <sstream>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <functional>
#include <iostream>

template<typename T>
class Matrix {
  typedef uint32_t u32;
  typedef int64_t  s64;

  template<typename U>
  using idcon = std::map<u32, U>;

  /* map of rows -> columns -> T. */
  idcon<idcon<T>> m;

  /* Dimensions. */
  u32 rows, cols;

  /* Contents of sparse areas. */
  T zero;

public:
  /* Construction, assignment, access, mutation. */
  Matrix(u32 r, u32 c, T z) : rows(r), cols(c), zero(z) {};
  Matrix(const Matrix & u) : m(u.m), rows(u.rows), cols(u.cols), zero(u.zero) {};
  Matrix & operator=(const Matrix & rhs) {
    if(this != &rhs) {
      rows = rhs.rows;
      cols = rhs.cols;
      zero = rhs.zero;
      m = rhs.m;
    }
    return *this;
  }
  const T & get(u32 i, u32 j) const {
    if(i >= rows || j >= cols) {
      throw std::runtime_error("Out-of-bounds Matrix::get()");
    }
    typename idcon<idcon<T>>::const_iterator row;
    typename idcon<T>::const_iterator ent;
    if(((row = m.find(i)) != m.end()) &&
       ((ent = row->second.find(j)) != row->second.end()))
    {
      return ent->second;
    } else {
      return zero;
    }
  }
  Matrix & set(u32 i, u32 j, const T & v) {
    if(i >= rows || j >= cols) {
      throw std::runtime_error("Out-of-bounds Matrix::set()");
    }
    m[i][j] = v;
    return *this;
  }
  /* Arithmetic. */
  Matrix & direct_op(const Matrix & rhs, std::function<T (const T&, const T&)> op) {
    if(cols != rhs.cols || rows != rhs.rows) {
      throw std::runtime_error("Matrix dimensions must agree");
    }
    typename idcon<idcon<T>>::iterator lrow;
    typename idcon<idcon<T>>::const_iterator rrow;
    for(lrow = m.begin(), rrow = rhs.m.cbegin();
        lrow != m.end() || rrow != rhs.m.cend();)
    {
      if(lrow == m.end() || (lrow->first > rrow->first)) {
        lrow = m.insert(lrow, *rrow);
        for (auto & nv : lrow->second) {
          nv.second = op(zero, nv.second);
        }
        if(lrow != m.end()) ++lrow;
        ++rrow;
      } else if(rrow == rhs.m.cend()) {
        break;
      } else if(lrow->first < rrow->first) {
        ++lrow;
      } else {
        typename idcon<T>::iterator le;
        typename idcon<T>::const_iterator re;
        for(le = lrow->second.begin(), re = rrow->second.cbegin();
            le != lrow->second.end() || re != rrow->second.cend();)
        {
          if(le == lrow->second.end() || (le->first > re->first)) {
            le = lrow->second.insert(le, *re);
            le->second = op(zero, le->second);
            if(le != lrow->second.end()) ++le;
            ++re;
          } else if(re == rrow->second.cend()) {
            break;
          } else if(le->first < re->first) {
            ++le;
          } else {
            le->second = op(le->second, re->second);
            ++le;
            ++re;
          }
        }
        ++lrow;
        ++rrow;
      }
    }
    return *this;
  }
  Matrix & operator+=(const Matrix & rhs) {
    return direct_op(rhs, [](const T & l, const T & r){ return l + r; });
  }
  Matrix & operator-=(const Matrix & rhs) {
    return direct_op(rhs, [](const T & l, const T & r){ return l - r; });
  }
  Matrix transpose() const {
    Matrix<T> t(cols,rows,zero);
    for(auto & row : m) {
      for(auto & e : row.second) {
        t.set(e.first, row.first, e.second);
      }
    }
    return t;
  }
  Matrix & identity(const T & unit) {
    m.clear();
    for(u32 i=0; i<cols && i<rows; i++)
    {
      idcon<T> row;
      row.insert(std::pair<int, T>(i, unit));
      m.insert(std::pair<int, idcon<T>>(i, row));
    }
    return *this;
  }
  Matrix operator*(const Matrix & rhs) const {
    if(cols != rhs.rows) {
      throw std::runtime_error("Matrix dimensions must agree");
    }
    Matrix<T> res(rows, rhs.cols, zero);
    const Matrix<T> rhst = rhs.transpose();
    bool nonzero;
    for(auto & lrow : m) {
      for(auto & rcol : rhst.m) {
        typename idcon<T>::const_iterator le,re;
        nonzero = false;
        T accum = zero;
        for(le = lrow.second.cbegin(), re = rcol.second.cbegin();
            le != lrow.second.cend() && re != rcol.second.cend();)
        {
          if(le->first == re->first) {
            nonzero = true;
            accum += le->second * re->second;
            le++;
            re++;
          } else if(le->first < re->first) {
            le++;
          } else {
            re++;
          }
        }
        if(nonzero) {
          res.set(lrow.first, rcol.first, accum);
        }
      }
    }
    return res;
  }
  Matrix & operator*=(const Matrix & rhs) {
    return (*this = *this * rhs);
  }
  Matrix & operator*=(const T & rhs) {
    for(auto & row : m) {
      for(auto & ent : row.second) {
        ent.second *= rhs;
      }
    }
    return *this;
  }
  Matrix operator*(const T & rhs) const {
    return (Matrix<T>(*this) *= rhs);
  }
  /* Scalar multiplication from left. */
  template <typename V>
  friend Matrix<V> operator*(const V & lhs, const Matrix<V> & rhs);

  Matrix & vappend(const Matrix & other) {
    if(cols != other.cols) {
      throw std::runtime_error("Matrix dimensions must agree");
    }
    for(auto & orow : other.m) {
      m.insert(std::pair<u32, idcon<T>>(orow.first + rows, orow.second));
    }
    rows += other.rows;
    return *this;
  }
  Matrix & happend(const Matrix & other) {
    if(rows != other.rows) {
      throw std::runtime_error("Matrix dimensions must agree");
    }
    for(auto & orow : other.m) {
      typename idcon<idcon<T>>::iterator lrow = m.find(orow.first);
      if(lrow == m.end()) {
        lrow = m.insert(std::pair<u32, idcon<T>>(orow.first, idcon<T>())).first;
      }
      for(auto & ent : orow.second) {
        lrow->second.insert(std::pair<u32, T>(ent.first + cols, ent.second));
      }
    }
    cols += other.cols;
    return *this;
  }
  /* Kronecker product. */
  Matrix operator%(const Matrix & rhs) const {
    Matrix<T> res(0, cols*rhs.cols, zero);
    s64 lastrow = -1;
    for(auto & lrow : m) {
      res.rows += (lrow.first - lastrow - 1)*rhs.rows;
      lastrow = lrow.first;
      Matrix<T> accum = Matrix<T>(rhs.rows, 0, zero);
      s64 lastcol = -1;
      for(auto & ent : lrow.second) {
        accum.cols += (ent.first - lastcol - 1)*rhs.cols;
        lastcol = ent.first;
        accum.happend(rhs * ent.second);
      }
      accum.cols = cols*rhs.cols;
      res.vappend(accum);
    }
    res.rows = rows*rhs.rows;
    return res;
  }
  Matrix & operator%=(const Matrix & rhs) {
    return (*this = *this % rhs);
  }
  std::string toString() {
    std::stringstream ss;
    s64 lrow = -1, lcol;
    for(auto & row : m) {
      for(;lrow+1 < row.first;lrow++) {
        for(u32 j=0;j<cols;j++) {
          ss << zero << ((j+1 < cols) ? "\t" : "");
        }
        ss << "\n";
      }
      lrow = row.first;
      lcol = -1;
      for(auto & e : row.second) {
        for(;lcol+1 < e.first;lcol++) {
          ss << zero << ((lcol+1 < cols) ? "\t" : "");
        }
        lcol = e.first;
        ss << e.second << ((e.first+1 < cols) ? "\t" : "");
      }
      for(;lcol+1 < cols;lcol++) {
        ss << zero << ((lcol+1 < cols) ? "\t" : "");
      }
      ss << "\n";
    }
    for(;lrow+1 < rows;lrow++) {
      for(u32 j=0;j<cols;j++) {
        ss << zero << ((j+1 < cols) ? "\t" : "");
      }
      ss << "\n";
    }
    return ss.str();
  }
  template <typename V>
  friend std::ostream & operator<<(std::ostream & s, const Matrix<V>& mat);
};

template <typename V>
Matrix<V> operator*(const V & lhs, const Matrix<V> & rhs) {
  return rhs * lhs;
}

template <typename V>
std::ostream & operator<<(std::ostream & s, const Matrix<V>& mat) {
  return (s << mat.toString());
}

