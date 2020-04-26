#pragma once

#include <kernel/kdef.h>
#include <string.h>
#include <kernel/data_structures/vector.hpp>

using kernel::data_structures::Vector;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(data_structures)

        template<typename T>
        class It 
        {
            private:
                T* data;
            public:
                It(T* data) {
                    this->data = data;
                }

                T &operator*() {
                    return *data;
                }

                It<T> &operator++() {
                    data++;
                    return *this;
                }

                bool operator==(const It<T> &a) {
                    return data == a.data;
                }

                bool operator!=(const It<T> &a) {
                    return data != a.data;
                }
        };

        class String 
        {
            private:
                char* data;
                size_t size;
            public:
                It<char> begin() {
                    It<char> it(data);
                    return it;
                }

                const char* c_str() const
                {
                    return data;
                }


                void operator+=(char c) 
                {
                    char arr[2];
                    arr[0] = c;
                    arr[1] = '\0';
                    this->add(arr);  
                }

                It<char> end()
                {
                    It<char> it(data + size);
                    return it;
                }

        String(size_t init_size) {
            this->data = new char[init_size];
            this->size = init_size;

            if (this->data != NULL) {
                for (size_t i = 0; i < init_size; i++) {
                    data[i] = '\0';
                }
            }
        }

        String(const Vector<char>& vec)
        {
            *this = String(vec.size() + 1);
            memcpy(this->data, const_cast<Vector<char>&>(vec).begin(), vec.size());
        }

        /**
        * Constructor with char*
        */
        String(const char* data) {
            size_t size = strlen(data);
            
            this->data = new char[size];
            this->size = size;

            if (this->data != NULL) {
                strncpy(this->data, data, size);
            }
        }

        String() : String("") {}

        /**
        * Copy Constructor
        */
        String(const String &string) {
            this->size = string.getLength();
            this->data = new char[this->size];

            if (this->data != NULL) {
                strncpy(this->data, string.data, this->size);
            }
        }

        String(String&& string)
        {
            this->data = string.data;
            this->size = string.size;
            string.data = nullptr; 
        }

        /**
        * Destructor
        */
        ~String() {
            if (this->data != NULL)
                delete[] this->data;
        }


        /** Methods **/


        void operator=(String&& string)
        {
            this->data = string.data;
            this->size = string.size;
            string.data = nullptr;
        }

        void operator=(const String& string)
        {
            this->size = string.getLength();
            this->data = new char[this->size];

            if (this->data != NULL) {
                strncpy(this->data, string.data, this->size);
            }
        }

        char* getText() {
            return data;
        }

        void setText(const char* text) {
            delete[] this->data;

            this->size = strlen(text);
            this->data = new char[this->size];

            strncpy(this->data, text, this->size);
        }

        size_t getLength() const {
            return strlen(this->data);
        }

        void setLength(size_t size) {
            size_t old_length = this->getLength();
            char* old_data = this->data;

            this->size = size;
            this->data = new char[size];

            // Copy the characters (or add \0 if there are no characters anymore).
            for (size_t i = 0; i < size; i++) {
                if (i < old_length) {
                    this->data[i] = old_data[i];
                } else {
                    this->data[i] = '\0';
                }
            }

            // Destroy the old data
            delete[] old_data;

            // Ensure that the last element is \0
            this->data[size] = '\0';
        }

        void add(const String &text) {
            size_t new_size = this->size + text.size;
            setLength(new_size);

            size_t length = getLength();
            for (size_t i = length; i < new_size; i++) {
                this->data[i] = text.data[i - length];
            }
        }

        char get(size_t pos) const {
            if (pos > getLength()) {
                return '\0';
            }
            return data[pos];
        }

        bool compare(const String &string) const {
            return strcmp(this->data, string.data) == 0;
        }

        char operator[](size_t pos) {
            return this->get(pos);
        }
    };

    NAMESPACE_END(data_structures)

NAMESPACE_END(kernel)

using kernel::data_structures::String;

/** Free Operator Methods **/

// String operator+(const String &string1, const String &string2) {
//     String string = String(string1);
//     string.add(string2);

//     return string;
// }

// bool operator==(const String &string1, const String &string2) {
//     return string1.compare(string2);
// }