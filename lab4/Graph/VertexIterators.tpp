#ifndef ITERATORS_TPP
#define ITERATORS_TPP

template <typename T>
class Graph<T>::VertexIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using iterator = typename std::list<VertexNode>::iterator;
    using const_iterator = typename std::list<VertexNode>::const_iterator;

    // Конструкторы для обычного и константного итератора
    explicit VertexIterator(iterator it) : current(it), is_const(false) {}
    explicit VertexIterator(const_iterator it) : current_const(it), is_const(true) {}

    // Доступ к элементу
    reference operator*() {
        return current->value; // Неконстантный доступ
    }

    const_reference operator*() const {
        return current_const->value; // Константный доступ
    }

    // Инкремент
    VertexIterator& operator++() {
        if (is_const) ++current_const;
        else ++current;
        return *this;
    }

    VertexIterator operator++(int) {
        VertexIterator temp = *this;
        ++(*this);
        return temp;
    }

    VertexIterator& operator--() {
        if (is_const) --current_const;
        else --current;
        return *this;
    }

    VertexIterator operator--(int) {
        VertexIterator temp = *this;
        --(*this);
        return temp;
    }

    // Сравнение
    // В классе VertexIterator
    bool operator==(const VertexIterator& other) const {
        // Если оба итератора - константные или оба неконстантные
        if (is_const == other.is_const) {
            return is_const ? current_const == other.current_const : current == other.current;
        }
        return false;
    }

    // Также перегрузите оператор для сравнения с std::_List_const_iterator
    bool operator==(const typename std::list<VertexNode>::const_iterator& other) const {
        return current_const == other;
    }


    bool operator!=(const VertexIterator& other) const {
        return !(*this == other);
    }

private:
    iterator current;
    const_iterator current_const;
    bool is_const;
};

// Reverse итератор для вершин
template <typename T>
class Graph<T>::VertexReverseIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using iterator = typename std::list<VertexNode>::reverse_iterator;
    using const_iterator = typename std::list<VertexNode>::const_reverse_iterator;

    explicit VertexReverseIterator(iterator it) : current(it), is_const(false) {}
    explicit VertexReverseIterator(const_iterator it) : current_const(it), is_const(true) {}

    // VertexIterators.tpp, строка 193
    reference operator*() {
        if (is_const) {
            return const_cast<T&>(current_const->value); // Приведение const к неконстантному типу
        } else {
            return current->value;
        }
    }

    // Для поддержки константных итераторов:
    const_reference operator*() const {
        return current_const->value; // Константная версия для безопасного чтения
    }


    VertexReverseIterator& operator++() {
        if (is_const) ++current_const;
        else ++current;
        return *this;
    }

    VertexReverseIterator operator++(int) {
        VertexReverseIterator temp = *this;
        ++(*this);
        return temp;
    }

    VertexReverseIterator& operator--() {
        if (is_const) --current_const;
        else --current;
        return *this;
    }

    VertexReverseIterator operator--(int) {
        VertexReverseIterator temp = *this;
        --(*this);
        return temp;
    }

    bool operator==(const VertexReverseIterator& other) const {
        return is_const == other.is_const &&
               (is_const ? current_const == other.current_const : current == other.current);
    }

    bool operator!=(const VertexReverseIterator& other) const {
        return !(*this == other);
    }

private:
    iterator current;
    const_iterator current_const;
    bool is_const;
};



#endif // ITERATORS_TPP


