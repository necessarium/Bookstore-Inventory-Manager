/*
Serendipity Booksellers is a small bookstore located in a shopping mall. They have a
cashier station equipped with a personal computer. The is the point-of-sale (POS) 
software package that makes the computer function as a cash register and keep an 
inventory file. The inventory file is be a database of all the books
in the bookstore. In general, the software performs the following tasks:

• Calculate the total of a sale, including sales tax
• When a book is purchased, subtract it from the inventory file
• Add, change, delete, and look up books in the inventory file
• Display various sales reports
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <map>
using namespace std;



const int CL_GAP = 200;


void print(string text, string& stream_record) {
	cout << text; stream_record += text;
}


void clear_stream(string& stream_record) {
    stream_record = ""; 
    print(string(CL_GAP, '\n'), stream_record);
}


void wait(double sec) {
	clock_t end = clock() + sec * CLOCKS_PER_SEC;
	while (clock() < end) {}
}


void show_wait_msg(string message, double wait_sec) {
    cout << message; wait(wait_sec);
}


string normalized_str(string str) {
    for(int i = 0; i < str.length(); i++)
        str[i] = tolower(str[i]);
    return str;
}


string split_tokens_str(string& tokens_str, string dl) {
    int f_dl_pos = tokens_str.find(dl); 
    string token = tokens_str.substr(0, f_dl_pos);
    tokens_str.erase(0, f_dl_pos + 1); return token;
}


void process_animation(double sec_speed, string action_str, 
                       const string& stream_record) {
	for (int i = 0; i < 3; i++) {
        cout << action_str; 
		for (int j = 0; j < 3; j++) {
            cout << '.'; wait(sec_speed);
		} cout << string(CL_GAP, '\n') << stream_record;
	}
}


bool is_double(string str) {
    bool str_len = (str.length()); if(!str_len) return false;
    bool is_deci_pt, is_digit, is_invalid_ch; int deci_pt_count = 0;
    for(int n = (str[0] == '-'); n < str_len; n++) {
        is_deci_pt = (str[n] == '.'); is_digit = isdigit(str[n]);
        if(!(is_deci_pt || is_digit)) return false;
        deci_pt_count += is_deci_pt; 
        if(deci_pt_count > 1 || is_invalid_ch) return false;
    } return true;
}


bool is_int(string str) {
	if(str.empty()) return false;
    int start = (str[0] == '-');
    for (int n = start; n < str.length(); n++) {
        if(!isdigit(str[n])) return false;
    } return true;
}


bool is_containing(string str_haystack, string substr_needle) {
    if(substr_needle.length() > str_haystack.length()) return false;
    int pos_upper_bound = str_haystack.length() - substr_needle.length();
    int found_pos = str_haystack.find(substr_needle);
    if(found_pos < 0 || found_pos > pos_upper_bound) return false;
    return true;
}


bool input_bool(string prompt, string& stream_record) {
    string user_input; print(prompt, stream_record);
    while(getline(cin, user_input)) {
        user_input = normalized_str(user_input);
        if(user_input == "yes" || user_input == "y" || 
           user_input == "1") return true;
        if(user_input == "no" || user_input == "n" || 
           user_input == "0") return false;
        cout << string(CL_GAP, '\n') << stream_record;
    }
}


enum {type_int, type_double};


template<typename numtype>
numtype input_num(int num_type, numtype sample, const string& stream_record) {
	string user_input; while (getline(cin, user_input)) {
        if(num_type == type_double && is_double(user_input)) return stod(user_input);
        else if(num_type == type_int && is_int(user_input)) return stoi(user_input);
        else cout << string(CL_GAP, '\n') << stream_record;
    }
}


template<typename numtype>
numtype input_num_within_range(int num_type, numtype min, numtype max, 
                               string prompt, string& stream_record) {
    print(prompt, stream_record); numtype num; while(true) {
        num = input_num(num_type, min + max, stream_record);
		if(num >= min && num <= max) {
			stream_record += to_string(num) + "\n";
			return num;
		} else {
            cout << "\nNumber must be between ";
            cout << min << " and " << max << "!"; wait(1.0);
            cout << string(CL_GAP, '\n') << stream_record;
		}
	}
}


double input_double_within_range(double min, double max, string prompt, string& stream_record) {
    return input_num_within_range(type_double, min, max, prompt, stream_record);
}


int input_int_within_range(int min, int max, string prompt, string& stream_record) {
    return input_num_within_range(type_int, min, max, prompt, stream_record);
}


void show_heading(string title, string subtitle, string& stream_record) {
    string formatted_title = "  " + title + "\n  ";
    int subtitle_padding_len = (title.length() - subtitle.length())/2;
    string subtitle_padding = string(subtitle_padding_len, ' ');
    string formatted_subtitle = subtitle_padding + subtitle + "\n\n";
    print(formatted_title + formatted_subtitle, stream_record);
}


void show_menu(string menu_items[], int menu_len, string& stream_record) {
    for(int i = 0; i < menu_len; i++) {
		string formatted_index = to_string(i+1) + ". ";
        string formatted_item = menu_items[i] + "\n";
        print(formatted_index + formatted_item, stream_record);   
    }
}


void show_menu_screen(string title, string subtitle, string menu_items[],  
                      int menu_len, string& stream_record, string custom_addition = "") {
    clear_stream(stream_record); show_heading(title, subtitle, stream_record);
    print(custom_addition, stream_record); show_menu(menu_items, menu_len, stream_record);
}


int input_menu_choice(int menu_len, string& stream_record) {
    return input_int_within_range(1, menu_len, "\nEnter Your Choice: ", stream_record);
}


void enter_to_continue(string message) {
    cout << message; string garbage; getline(cin, garbage);
}


string money_format(double amount) {
    string str_amount = to_string(round(amount * 100) / 100);
    str_amount = str_amount.substr(0, str_amount.find(".") + 3);
    return "$" + str_amount;
}


enum {by_title, by_ISBN, by_author, by_publisher, by_quantity, by_wholesale, by_retail};


class BookData {
    protected:
        string title, ISBN, author, publisher;

    public:
        BookData(string titl, string isbn, string auth, string pub) {
            title = titl; ISBN = isbn; author = auth; publisher = pub; 
        }

        BookData(const BookData& book_data) { 
            title = book_data.title; 
            ISBN = book_data.ISBN; 
            author = book_data.author; 
            publisher = book_data.publisher; 
        }


        void set_title(string titl) {title = titl;}
        void set_ISBN(string isbn) {ISBN = isbn;}
        void set_author(string auth) {author = auth;}
        void set_publisher(string pub) {publisher = pub;}

        string get_title() const {return title;}
        string get_ISBN() const {return ISBN;}
        string get_author() const {return author;}
        string get_publisher() const {return publisher;}

        string get_str_attr(int attr_num) {
            switch(attr_num) {
                case by_title: return title;
                case by_ISBN: return ISBN;
                case by_author: return author;
                case by_publisher: return publisher;
            }
        }
};


class InventoryBook : public BookData {
    protected:
        string date_added;
        int quantity;
        double wholesale;
        double retail;

    private:
        bool is_in_cart;
        string dl = "|";

    public:
        InventoryBook(string titl = "", string isbn = "", string auth = "", 
                      string pub = "", string date = "", int qty = 0, double wholsal = 0, 
                      double ret = 0) : BookData(titl, isbn, auth, pub) {
            date_added = date; quantity = qty; wholesale = wholsal; 
            retail = ret; is_in_cart = false;
        }

        InventoryBook(const InventoryBook& inv_book) : BookData(inv_book) {
            date_added = inv_book.date_added; quantity = inv_book.quantity; 
            wholesale = inv_book.wholesale; retail = inv_book.retail; 
            is_in_cart = inv_book.is_in_cart;
        }

        InventoryBook operator -= (int subtract_amount) {
            this->quantity -= subtract_amount;
            return *this;
        }

        void set_date_added(string date) {date_added = date;}
        void set_quantity(int qty) {quantity = qty;}
        void set_wholesale(double wholsal) {wholesale = wholsal;}
        void set_retail(double ret) {retail = ret;}
        void set_is_in_cart(bool is_in) {is_in_cart = is_in;}

        string get_date_added() {return date_added;}
        int get_quantity() {return quantity;}
        double get_wholesale() {return wholesale;}
        double get_retail() {return retail;}
        bool get_is_in_cart() {return is_in_cart;}  
        string get_dl() {return dl;} 

        double get_num_attr(int attr_num) {
            switch(attr_num) {
                case by_quantity: return quantity; 
                case by_wholesale: return wholesale; 
                case by_retail: return retail; 
            }
        }

        string get_db_format() {
            string db_formatted = "";
            db_formatted += title + dl + ISBN + dl + author + dl + publisher;
            db_formatted += dl + date_added + dl + to_string(quantity) + dl;
            db_formatted += to_string(wholesale) + dl + to_string(retail);
            return db_formatted;
        }

        void remove_book() {
            set_quantity(0); set_wholesale(0); set_retail(0);
            set_title(""); set_ISBN(""); set_author(""); 
            set_publisher(""); set_date_added("");
        }

        bool is_empty() {
            return (title == "" && ISBN == "" && 
                    author == "" && publisher == "" && 
                    date_added == "" && quantity == 0);
        }

        void display_book_info(int bar_size, string& stream_record) {
            print("  " + string(bar_size, '-'), stream_record);
            print("\n  Title: " + title, stream_record);
            print("\n  ISBN: " + ISBN, stream_record);
            print("\n  Author: " + author, stream_record);
            print("\n  Publisher: " + publisher, stream_record);
            print("\n  Date Added: " + date_added, stream_record);
            print("\n  Quantity: " + to_string(quantity), stream_record);
            print("\n  Wholsale: " + money_format(wholesale), stream_record);
            print("\n  Retail: " + money_format(retail), stream_record);
            print("\n  " + string(bar_size, '-'), stream_record);
        }
};
    

class SoldBook : public InventoryBook {
    private:
        int inv_book_idx, quantity;

    public: 
        SoldBook(const InventoryBook& inv_book, 
                 int book_idx, int qty) : InventoryBook(inv_book) {
            inv_book_idx = book_idx; quantity = qty;  
        }

        int get_inv_book_idx() {return inv_book_idx;}
        int get_quantity() {return quantity;}
        int get_total_retail() {return quantity * retail;}
        void set_quantity(int qty) {quantity = qty;}
};


struct Data {
    string stream_record = string(200, '\n');
    const string FILE_NAME = "inventory_file.txt";
    static const int BOOKS_LEN = 20;
    InventoryBook books[BOOKS_LEN];
    vector<SoldBook> cart;
    fstream file_obj;

    int lookup_book_helper(int choice_lookup_by, string title);
    int lookup_book(string subtitle = "Lookup Book", 
                    string back_go_to = "Inventory Menu",
                    string custom_addition = "");
    void add_book(), edit_book(), delete_book(); 
    void report(int choice_report_by); //ntdn
    void load_books(), dump_books();
    bool checkout(string cashier_books_str); 
    string get_cart_record_str();
    void edit_qty_in_cart(), remove_from_cart(), clear_cart();
    void add_to_cart(string back_go_to, string cashier_books_str);

    Data() {load_books();}
    ~Data() {dump_books();}
};


void Data::load_books() {
    file_obj.open(FILE_NAME, ios::in);
    if(!file_obj) throw "ERROR: File cannot be opened for reading.";
    string db_book_str, dl = books[0].get_dl(); int book_idx = 0;
    while(getline(file_obj, db_book_str)) {
        books[book_idx].set_title(split_tokens_str(db_book_str, dl));
        books[book_idx].set_ISBN(split_tokens_str(db_book_str, dl));
        books[book_idx].set_author(split_tokens_str(db_book_str, dl));
        books[book_idx].set_publisher(split_tokens_str(db_book_str, dl));
        books[book_idx].set_date_added(split_tokens_str(db_book_str, dl));
        books[book_idx].set_quantity(stoi(split_tokens_str(db_book_str, dl)));
        books[book_idx].set_wholesale(stod(split_tokens_str(db_book_str, dl)));
        books[book_idx].set_retail(stod(split_tokens_str(db_book_str, dl)));
        book_idx++; if(book_idx > BOOKS_LEN) break;
    } file_obj.close();
}

void Data::dump_books() {
    file_obj.open(FILE_NAME, ios::out);
    for(int book_idx = 0; book_idx < BOOKS_LEN; book_idx++) {
        if(books[book_idx].is_empty()) continue;
        file_obj << books[book_idx].get_db_format() << endl;
    } file_obj.close();
}


void do_nothing(Data& program_data) {return; } 


string input_ISBN(string& stream_record) {}


string input_str_field(string field_prompt, string& stream_record) {
    print(field_prompt, stream_record); string str_field;
    while(getline(cin, str_field)) {
        if(!str_field.empty()) {
            stream_record += str_field  + "\n";
            return str_field;
        } else {
            cout << "Field cannot be empty!"; wait(1.0);
            cout << string(CL_GAP, '\n') << stream_record;
        }
    }
}


string input_date() {}


int input_int_field(string field_prompt, string& stream_record) {
    return input_int_within_range(1, 9999, field_prompt, stream_record);
}


double input_double_field(string field_prompt, string& stream_record) {
    return input_double_within_range(0, 9999, field_prompt, stream_record);
}


InventoryBook input_book(string& stream_record) {
    string ISBN = input_str_field("ISBN: ", stream_record);
    string title = input_str_field("Title: ", stream_record);
    string author = input_str_field("Author: ", stream_record);
    string publisher = input_str_field("Publisher: ", stream_record);
    string date_added = input_str_field("Date Added: ", stream_record);
    int quantity = input_int_field("Quantity: ", stream_record);
    double wholesale = input_double_field("Wholesale Cost: $", stream_record);
    double retail = input_double_field("Retail Price: $", stream_record);    

    InventoryBook book(title, ISBN, author, publisher, 
                       date_added, quantity, wholesale, retail);
    return book;
}


void Data::add_book() {
    clear_stream(stream_record);
    string title = "Serendipity Booksellers", subtitle = "Add Book";
    show_heading(title, subtitle, stream_record);
    for(int idx = 0; idx < BOOKS_LEN; idx++) {
        if(!books[idx].is_empty()) continue;
        books[idx] = input_book(stream_record);
        dump_books(); return;
    } enter_to_continue("\nNo more books may be added.");
}


int Data::lookup_book_helper(int choice_lookup_by, string title) {
    map<int, string> options_lookup_by = {{by_title, "Title"}, {by_ISBN, "ISBN"}, 
                                          {by_publisher, "Publisher"}, {by_author, "Author"}};
    string prompt = "\nEnter Book " + options_lookup_by[choice_lookup_by] + ": ";
    string substr_needle = normalized_str(input_str_field(prompt, stream_record));
    string str_haystack, option_header, last_option;
    InventoryBook book; vector<int> matched_book_idxs;
    int last_option_num, num_matched_books = 0;

    for(int book_idx = 0; book_idx < BOOKS_LEN; book_idx++) {
        book = books[book_idx]; if(book.is_empty()) continue;
        str_haystack = normalized_str(book.get_str_attr(choice_lookup_by));
        if(!is_containing(str_haystack, substr_needle)) continue;
        matched_book_idxs.push_back(book_idx);
        option_header = "\n\nOption " + to_string(++num_matched_books) + ")\n";
        print(option_header, stream_record);
        book.display_book_info(title.length(), stream_record);
    }

    if(num_matched_books == 0) {
        cout << "\n\nNo match found in inventory."; wait(1.0); return -1;
    } else {
        option_header = "\n\nOption " + to_string(num_matched_books + 1) + ")\n";
        last_option = "  |Return to Lookup Menu|\n"; last_option_num = num_matched_books + 1;
        print(option_header + last_option, stream_record);
        int choice = input_menu_choice(last_option_num, stream_record);  
        if(choice == last_option_num) return -1; else return matched_book_idxs[choice - 1];
    }
}


int Data::lookup_book(string subtitle, string back_go_to, string custom_addition) {
    string title = "Serendipity Booksellers"; int choice_lookup_by, book_idx = -1;
    string lookup_options_menu_items[] = {"Lookup by Title", "Lookup by ISBN", "Lookup by Author", 
                                          "Lookup by Publisher", "Return to " + back_go_to};
    int lookup_options_items[] = {by_title, by_ISBN, by_author, by_publisher};
    int lookup_menu_len = sizeof(lookup_options_menu_items)/sizeof(lookup_options_menu_items[0]);
    do {show_menu_screen(title, subtitle, lookup_options_menu_items, 
                         lookup_menu_len, stream_record, custom_addition);
        int choice = input_menu_choice(lookup_menu_len, stream_record);
        if(choice == lookup_menu_len) return -1; 
        choice_lookup_by = lookup_options_items[choice - 1];
        book_idx = lookup_book_helper(choice_lookup_by, title);
    } while(book_idx == -1);
    
    return book_idx;
}


void Data::edit_book() {
    string title = "Serendipity Booksellers", subtitle = "Edit Book";
    int edit_book_index = lookup_book(subtitle); if(edit_book_index == -1) return;
    if(books[edit_book_index].get_is_in_cart()) {
        print("\nBook is in cart. If you proceed, cart will be cleared.", stream_record);
        if(input_bool("\nDo you want to proceed? ", stream_record)) cart.clear(); else return;
    }
    string& sr = stream_record; InventoryBook& book = books[edit_book_index];
    string edit_options_menu_items[] = {"Edit ISBN", "Edit Title", "Edit Author", "Edit Publisher", 
                                        "Edit Date Added", "Edit Quantity", "Edit Wholesale Cost", 
                                        "Edit Retail Price", "Return to Inventory Menu"};
    int edit_options_menu_len = sizeof(edit_options_menu_items)/sizeof(edit_options_menu_items[0]);
    int choice; while(true) {
        clear_stream(sr); show_heading(title, subtitle, sr);
        book.display_book_info(title.length(), sr); print("\n\n", sr);
        show_menu(edit_options_menu_items, edit_options_menu_len, sr);
        choice = input_menu_choice(edit_options_menu_len, sr);         
        switch(choice) {
            case 1: book.set_ISBN(input_str_field("\nNew ISBN: ", sr)); break;
            case 2: book.set_title(input_str_field("\nNew Title: ", sr)); break;  
            case 3: book.set_author(input_str_field("\nNew Author: ", sr)); break;  
            case 4: book.set_publisher(input_str_field("\nNew Publisher: ", sr)); break;  
            case 5: book.set_date_added(input_str_field("\nNew Date Added: ", sr)); break;  
            case 6: book.set_quantity(input_int_field("\nNew Quantity: ", sr)); break;  
            case 7: book.set_retail(input_double_field("\nNew Retail Price: $", sr)); break;  
            case 8: book.set_wholesale(input_double_field("\nNew Wholesale Cost: $", sr)); break;  
            case 9: return;
        } dump_books();
    }
}


void Data::delete_book() {
    string subtitle = "Delete Book";
    int delete_book_index = lookup_book(subtitle);
    if(delete_book_index == -1) return; 
    if(books[delete_book_index].get_is_in_cart()) {
        string s1 = "The book is in cart. ";
        string s2 = "Deletion will cause cart to be cleared.";
        print(s1 + s2, stream_record); 
    } if(input_bool("\nAre you sure? ", stream_record)) {
        books[delete_book_index].remove_book(); cart.clear();
        process_animation(0.14, "Deleting", stream_record);
        dump_books();
    }
}


string text_field(string text, int field_size,int space_count, bool& is_break, string& broken_line) {
    bool isb_dum = false; string bl_dum = "";
    if(text.length() > field_size) {
        string broken_str = text.substr(field_size); is_break = true;
        broken_line += text_field(broken_str, field_size, space_count, isb_dum, bl_dum);
        return text.substr(0, field_size) + string(space_count, ' '); 
    }
    broken_line += string(field_size + space_count, ' ');
    return text + string(field_size + space_count - text.length(), ' ');
}


string five_fields_line(string t1, string t2, string t3, string t4, string t5) {
    int a = 4, b = 17, c = 16, d = 8, space_count = 3;
    string fields_line = "", broken_line = "\n"; bool is_break = false;
    fields_line += text_field(t1, a, space_count, is_break, broken_line);
    fields_line += text_field(t2, b, space_count, is_break, broken_line);
    fields_line += text_field(t3, c, space_count, is_break, broken_line);
    fields_line += text_field(t4, d, space_count, is_break, broken_line);
    fields_line += text_field(t5, t5.length(), space_count, is_break, broken_line);
    fields_line += (is_break ? broken_line : "") + "\n";
    return fields_line;
}


bool Data::checkout(string cashier_books_str) {
    if(cart.empty()) {
        enter_to_continue("\nCart is Empty <> Press Enter to Return to Cashier Menu\n\n"); 
        return false; }
    if(!input_bool("\nDo you want to checkout now? ", stream_record)) return false;
    clear_stream(stream_record); print("Serendipity Booksellers\n\n", stream_record);
    print(cashier_books_str, stream_record); double sales_tax_rate = 0.06, subtotal = 0;
    for(SoldBook cart_book : cart) {subtotal += cart_book.get_total_retail(); 
                                    cart_book-= cart_book.get_quantity();}
    double tax = sales_tax_rate * subtotal, total = subtotal + tax;
    print(five_fields_line("", "", "Subtotal", "", money_format(subtotal)), stream_record);
    print(five_fields_line("", "", "Tax", "", money_format(tax)), stream_record);
    print(five_fields_line("", "", "Total", "", money_format(total)), stream_record);
    print("\n\nThank you for shopping at Seredipity Booksellers!", stream_record);
    enter_to_continue("\n\nCheckout Complete <> Press Enter to Return to Main Menu\n\n");
    return true;
}


void Data::add_to_cart(string back_go_to, string cashier_books_str) {
    int book_idx; while(true) { 
        book_idx = lookup_book("Add to Cart", back_go_to, cashier_books_str); if(book_idx == -1) return; 
        if(!books[book_idx].get_is_in_cart()) {books[book_idx].set_is_in_cart(true); break;} 
        else {print("Book is already in cart.", stream_record); wait(1.0);}
    } InventoryBook& book = books[book_idx]; string& sr = stream_record;
    int book_qty = input_int_within_range(1, book.get_quantity(), "\nBook Quantity: ", sr);
    cart.push_back(SoldBook(book, book_idx, book_qty)); 
    process_animation(0.10, "Adding", stream_record);
}


void Data::edit_qty_in_cart() {
    if(cart.empty()) {enter_to_continue("\nCart is Empty <> Press Enter to Return to Cashier Menu\n\n"); return;}
    SoldBook& cart_book = cart[input_int_within_range(1, cart.size(), "\nEnter Book Choice: ", stream_record) - 1];
    InventoryBook& inv_book = books[cart_book.get_inv_book_idx()]; string& sr = stream_record;
    int new_book_qty = input_int_within_range(1, inv_book.get_quantity(), "\nNew Book Quantity: ", sr);
    cart_book.set_quantity(new_book_qty);
}


void Data::remove_from_cart() {
    if(cart.empty()) {enter_to_continue("\nCart is Empty <> Press Enter to Return to Cashier Menu\n\n"); return;}
    int cart_delete_choice = input_menu_choice(cart.size(), stream_record);
    cart.erase(cart.begin() + cart_delete_choice - 1); 
    process_animation(0.10, "Removing", stream_record);
}


void Data::clear_cart() {
    print("\nAll items in cart will be cleared. ", stream_record);
    if(input_bool("\nAre you sure?  ", stream_record)) cart.clear();
}


string Data::get_cart_record_str() {
    string cart_record_str = "";
    string q, is, t, r, tr;
    for(SoldBook cart_book : cart) {
        q = to_string(cart_book.get_quantity());
        is = cart_book.get_ISBN(); t = cart_book.get_title();
        r = money_format(cart_book.get_retail());
        tr = money_format(cart_book.get_total_retail());
        cart_record_str += five_fields_line(q, is, t, r, tr);
    }
    return cart_record_str;
}


void show_cashier_screen(Data& program_data) {
    string title = "Serendipity Booksellers", subtitle = "Cashier Module", back_go_to = "Cashier Menu";
    string cashier_menu_items[] = {"Checkout", "Add a Book", "Edit Book Quantity", 
                                   "Remove a Book", "Clear Cart", "Return to the Main Menu"};
    int cashier_menu_len = sizeof(cashier_menu_items)/sizeof(cashier_menu_items[0]); 
    string checkout_books_header_str = five_fields_line("Qty", "ISBN", "Title", "Price", "Total");
    checkout_books_header_str += string(checkout_books_header_str.length() - 1, '-') + "\n"; 
    string cashier_books_str, cart_books_record_str = program_data.get_cart_record_str();
    bool is_checkout = false; clear_stream(program_data.stream_record);
    do {cashier_books_str = checkout_books_header_str + cart_books_record_str + "\n\n";
        show_menu_screen(title, subtitle, cashier_menu_items, cashier_menu_len, 
                         program_data.stream_record, cashier_books_str);
        int choice = input_menu_choice(cashier_menu_len, program_data.stream_record);
        switch(choice) {
            case 1: is_checkout = program_data.checkout(cashier_books_str); break;
            case 2: program_data.add_to_cart(back_go_to, cashier_books_str); break;
            case 3: program_data.edit_qty_in_cart(); break;
            case 4: program_data.remove_from_cart(); break;
            case 5: program_data.clear_cart(); break;
            case 6: return;
        } cart_books_record_str = program_data.get_cart_record_str();
    } while(!is_checkout);
}


bool inventory_menu_navigation_controller(const int& inventory_menu_len, Data& program_data) {
    int choice = input_menu_choice(inventory_menu_len, program_data.stream_record);
    switch(choice) {
        case 1: program_data.lookup_book(); break;
        case 2: program_data.add_book(); break;
        case 3: program_data.edit_book(); break;
        case 4: program_data.delete_book(); break;
        case 5: return false; 
    } return true;
}


void show_inventory_menu_screen(Data& program_data) {
    string title = "Serendipity Booksellers", subtitle = "Inventory Database";
    string inventory_menu_items[] = {"Lookup a Book", "Add a New Book", "Edit a Book's Record",
                                     "Delete a Book", "Return to the Main Menu"};
    int inventory_menu_len = sizeof(inventory_menu_items)/sizeof(inventory_menu_items[0]);
    do { show_menu_screen(title, subtitle, inventory_menu_items, 
                          inventory_menu_len, program_data.stream_record);
    } while(inventory_menu_navigation_controller(inventory_menu_len, program_data));
}



/*
The Report module analyzes the information in the Inventory Database to produce the following reports:
• Inventory List. A list of information on all books in the inventory.
• Inventory Wholesale Value. A list of the wholesale value of all books in the inventory and the total wholesale value of the inventory.
• Inventory Retail Value. A list of the retail value of all books in the inventory and the total retail value of the inventory.
• List by Quantity. A list of all books in the inventory sorted by quantity on hand. The books with the greatest quantity on hand will be listed first.
• List by Cost. A list of all books in the inventory, sorted by wholesale cost. The books with the greatest wholesale cost will be listed first.
• List by Age. A list of all books in the inventory, sorted by purchase date. The books that have been in the inventory longest will be listed first.


void report_by_wholesale()
void report_by_retail()
void repoprt_by_quantity()
void repoprt_by_cost()
void repport_by_age()

void report(Data& program_data) {
    vector<InventoryBook> a;
    for(int book_idx; book_idx < program_data.BOOKS_LEN; book_idx++)
        if(!program_data.books[book_idx].is_empty())
            a.push_back(program_data.books[book_idx])
}*/


//untested, unrefined
void report_by_listings(Data& program_data) {
    string& sr = program_data.stream_record;
    for(int book_idx = 0; book_idx < program_data.BOOKS_LEN; book_idx++) {
        if(!program_data.books[book_idx].is_empty()) { print("\n\n", sr);
            program_data.books[book_idx].display_book_info(23, sr);
        }
    } enter_to_continue("\n\nPress Enter to Return to Reports Menu\n\n");
}


//untested, unrefined
void Data::report(int choice_report_by) {
    InventoryBook* books_ptrs[BOOKS_LEN];
    for(int book_idx = 0; book_idx < BOOKS_LEN; book_idx++) {
        books_ptrs[book_idx] = &books[book_idx];
    }
    
    InventoryBook* s; int max_index;
    for(int left_bound = 0; left_bound < BOOKS_LEN - 1; left_bound++) {
        max_index = left_bound;
        for(int j = left_bound + 1; j < BOOKS_LEN; j++) {
            if(books_ptrs[j]->get_num_attr(choice_report_by) > 
               books_ptrs[max_index]->get_num_attr(choice_report_by))
                max_index = j;
        }
        s = books_ptrs[max_index];
        books_ptrs[max_index] = books_ptrs[left_bound];
		books_ptrs[left_bound] = s;
    }

    string& sr = stream_record;
    for(int book_idx = 0; book_idx < BOOKS_LEN; book_idx++) {
        if(!(*books_ptrs[book_idx]).is_empty()) {
            InventoryBook& book = *books_ptrs[book_idx]; 
            print("\n\n", sr); book.display_book_info(23, sr);
        }
    }
    enter_to_continue("\n\nPress Enter to Return to Reports Menu\n\n");
}

    

bool reports_menu_navigation_controller(const int& reports_menu_len, Data& program_data) {
    int choice = input_menu_choice(reports_menu_len, program_data.stream_record);
    switch(choice) {
        case 1: report_by_listings(program_data); break;
        case 2: 
        case 3: 
        case 4: do_nothing(program_data); break;
        case 5: program_data.report(by_quantity); break;
        case 6: program_data.report(by_wholesale); break;
        case 7: program_data.report(by_retail); break;
        case 8:
        case 9: return false;
    } return true;
}


void show_reports_menu_screen(Data& program_data) {
    string title = "Serendipity Booksellers", subtitle = "Reports";
    string reports_menu_items[] = {"Inventory Listing", "Inventory Wholesale Value",
                                   "Inventory Retail Value", "Inventory Quantity", 
                                   "Listing by Quantity", "Listing by Wholesale Cost", 
                                   "Listing by Retail Cost", "Listing by Age", 
                                   "Return to Main Menu"};
    int reports_menu_len = sizeof(reports_menu_items)/sizeof(reports_menu_items[0]);
    do { show_menu_screen(title, subtitle, reports_menu_items, 
                          reports_menu_len, program_data.stream_record);
    } while(reports_menu_navigation_controller(reports_menu_len, program_data));
}


bool main_menu_navigation_controller(const int& main_menu_len, Data& program_data) {
    int choice = input_menu_choice(main_menu_len, program_data.stream_record);
    switch(choice) {
        case 1: show_cashier_screen(program_data); break;
        case 2: show_inventory_menu_screen(program_data); break;
        case 3: show_reports_menu_screen(program_data); break;
        case 4: return false;
    } return true;
}


void show_main_menu_screen(Data& program_data) {
    string title = "Serendipity Booksellers", subtitle = "Main Menu";
    string main_menu_items[] = {"Cashier Module", "Inventory Database Module", 
                                "Report Module", "Exit"};
    int main_menu_len = sizeof(main_menu_items)/sizeof(main_menu_items[0]);
    do { show_menu_screen(title, subtitle, main_menu_items,     
                          main_menu_len, program_data.stream_record);
    } while(main_menu_navigation_controller(main_menu_len, program_data));
}


int main() {
    Data program_data; 
    show_main_menu_screen(program_data);
}