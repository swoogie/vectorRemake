#include <memory>
#include <iostream>
#include <chrono>
#include <vector>

template <class T> 
class Vector 
{
    public:
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef size_t size_type;
        typedef T value_type;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        Vector() { create(); }
        
        explicit Vector(size_type n, const T& val = T{}) { create(n, val); }

        Vector(const Vector& v){create(v.begin(), v.end()); }

        Vector& operator=(const Vector& rhs)
        {
            if(&rhs != this)
            {
                uncreate();

                create(rhs.begin(), rhs.end());
            }
            return *this;
        }
        ~Vector(){uncreate(); }

        iterator begin() {return data; }
        const_iterator begin() const { return data; }

        iterator end() {return dataEnd; }
        const_iterator end() const { return dataEnd; }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin() const { return const_reverse_iterator(end()); }
        
    // size funkcija
        size_type size(){ return limit - data; }
        size_type max_size(){ return alloc.max_size;}
        void resize(size_type n, const T& val = T{})
        {
            if( n > size() ){
                std::cout<<n-size()<<"\n";
                insert(end(), n-size(),val);
            }
            else if (n< size()){
                erase(begin()+n, end());
            }
        }
            size_type capacity() const noexcept{
                return limit-data;
            }
            bool empty() const noexcept{
                return data-dataEnd==0;
            }
            void reserve (size_type n){
                if(n > capacity()){
                    grow(n);
                }
            }
            void shrink_to_size(){
                grow(size());
            }

        T& operator[](size_type i) { return data[i]; }
        const T& operator[](size_type i) const { return data[i]; }
    
        T& at(size_type n){ return data[n]; }
        const T& at (size_type n) const{ return data[n]; }
    
        T& head(){ return *data; }
        const T& head() const { return *data; }

        T& tail(){ return *(dataEnd - 1); }
        const T& tail() const{ return *(dataEnd - 1); }
    
        T* data1() noexcept{ return data; }
        const T* data1() const noexcept{ return data; }

        void push_back(const T& val){
        if (dataEnd == limit) // išskirti vietos, jei reikia
        grow(); // TODO: reikia realizuoti
        unchecked_append(val) ; // įdėti naują elementą
        }
    
        void pop_back(){
            alloc.destroy(dataEnd - 1);
        }
    
        void assign(size_type n, const T& val){
            uncreate();
            create(n, val);
        }

        template <class InputIterator>
        void assign (InputIterator first, InputIterator last){
            uncreate();
            create(first,last);
        }
    
        iterator erase(iterator position){
            if(position < begin() || position > end())
            {
                return position;
            }
            else
            {
                alloc.destroy(position);
                dataEnd = std::uninitialized_copy(position + 1, end(), position);
                return position;
            }
        }

        iterator erase(iterator first, iterator last){
            if (first < begin() || last > end()) { return first; } 
            else
            {
                iterator it = last;
                while(it != first) alloc.destroy(--it); //delete elements in range [first,last)

                dataEnd = std::uninitialized_copy(last, end(), first); // copy elements from [last,end())

                return first+1; 
            }
        }
     
        void clear(){
            uncreate();
        }

        iterator insert(iterator position, const T& val){
            if(position < data || position > dataEnd){ return data; } //should add error
            else{
                int index = std::distance(data,position);
                if(dataEnd == limit) grow(); //changes pointer
                push_back(*(dataEnd-1));
                for(size_t i= size()-1; i>index; i-- ){
                    data[i]= data[i-1];
                }
                data[index] = val;

                return &data[index];
            }
        }

        iterator insert (iterator position, size_type n, const T& val){
            for(size_t i = 0; i< n; i++){
                position = insert(position,val);
            }
            return position;
        }
    private:
        iterator data; 
        iterator dataEnd;
        iterator limit; 

        std::allocator<T> alloc;

    void create(){
        data = dataEnd = limit = nullptr;
    }

    void create(size_type n, const T& val){
        data = alloc.allocate(n);
        limit = dataEnd = data + n;
        unintialized_fill(data, dataEnd, val);
    }

    void create(const_iterator i, const_iterator j){
        data = alloc.allocate(j-i);
        limit = dataEnd = std::uninitialized_copy(i, j, data);
    }

    void uncreate(){
        if (data)
        {
            iterator it = dataEnd;
            while(it != data)
            {
                alloc.destroy(it);
                it--;
            }
            alloc.deallocate(data, limit - data);
        }
        data = limit = dataEnd = nullptr;
    }

    void grow(){
        size_type newSize = std::max(2 * (limit - data), ptrdiff_t(1));

        iterator newPlace = alloc.allocate(newSize);
        iterator newLast = std::uninitialized_copy(data, dataEnd, newPlace);

        uncreate();

        data = newPlace;
        dataEnd = newLast;
        limit = data + newSize;
    }

    void grow(size_type newSize){
        iterator newPlace = alloc.allocate(newSize);
        iterator newLast = std::uninitialized_copy(data, dataEnd, newPlace);

        uncreate();

        data = newPlace;
        dataEnd = newLast;
        limit = data + newSize;
    }

    void unchecked_append(const T& val){
        alloc.construct(dataEnd++, val);
    }
};

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    unsigned int sz = 100000000;  
    std::vector<int> v1;
    for (int i = 1; i <= sz; ++i)
        v1.push_back(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end-start;
    std::cout << "std::vector uztruko: " << diff.count() << "s\n";

    auto start1 = std::chrono::high_resolution_clock::now();
    Vector<int> v2;
    for (int i = 1; i <= sz; ++i)
        v2.push_back(i);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff1 = end1-start1;
    std::cout << "Vector uztruko: " << diff1.count() << "s\n";
}
