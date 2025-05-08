#include <array>
#include <cstdio>
#include <queue>
#include <stack>

template <typename T, size_t S> class fixed_vector : public std::array<T, S> { 

  public: 
    void push_back(const T &el) { 
        if(cnt_ < S) { 
            this->at(cnt_) = el; 
            ++cnt_; 
        } 
    } 

    T &back() { 
        return this->at(cnt_-1); 
    } 

    void pop_back() { 
        if(cnt_) { 
            --cnt_; 
        } 
    } 

    auto end() { 
        return std::array<T, S>::begin() + cnt_; 
    } 

    bool empty() const { 
        return cnt_ == 0; 
    } 

  private: 
    size_t cnt_ = 0; 
}; 

int main() { 

    std::priority_queue<int, fixed_vector<int, 10>> pq; 

    pq.push(10); 
    pq.push(4); 
    pq.push(8); 
    pq.push(1); 
    pq.push(2); 

    printf("Popping elements from priority queue: "); 

    while(!pq.empty()) { 
       printf("%d ", pq.top()); 
       pq.pop(); 
    } 

    std::stack<int, fixed_vector<int, 10>> st; 

    st.push(10); 
    st.push(4); 
    st.push(8); 
    st.push(1); 
    st.push(2); 

    printf("\r\nPopping elements from stack (LIFO): "); 

    while(!st.empty()) { 
       printf("%d ", st.top()); 
       st.pop(); 
    } 
    printf("\r\n");
  
    return 0; 
}
