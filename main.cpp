// Thư viện xử lý file
#include <fstream>
// Thư viện xử lý chuỗi
#include <sstream>
// Thư viện mảng động (danh sách)
#include <vector>
// Thư viện map (bảng tra cứu key-value)
#include <map>
// Thư viện string
#include <string>
// Thư viện nhập xuất
#include <iostream>
// Thư viện xử lý thời gian
#include <ctime>
// Thư viện định dạng output
#include <iomanip>

// Sử dụng namespace std để không cần viết std:: trước mỗi lệnh
using namespace std;

class Account {
public:
    string username;
    string password;
    string email;
    string fullName;
    string role;

    Account(string u = "", string p = "", string e = "", string f = "")
        : username(u), password(p), email(e), fullName(f) {}

    virtual bool login(const string &u, const string &p) {
        return username == u && password == p;
    }

    virtual void logout() { /* no-op */ }
    virtual ~Account() = default;
};

class Answer;

class Question {
private:
    static int nextQuestionID;
public:
    int questionID;
    string questionText;
    int points;
    vector<Answer*> answers;

    Question(string text = "", int pts = 1) : questionText(text), points(pts) {
        questionID = ++nextQuestionID;
    }

    Answer* addAnswer(string text, bool isCorrect);
    bool checkAnswer(int answerID);
};

class Answer {
private:
    static int nextAnswerID;
public:
    int answerID;
    string answerText;
    bool isCorrect;
    
    Answer(string text = "", bool correct = false) : answerText(text), isCorrect(correct) { 
        answerID = ++nextAnswerID; 
    }
    
    int getID() const { return answerID; }
    string getText() const { return answerText; }
    bool correct() const { return isCorrect; }
};

class QuizResult;

class Quiz {
private:
    static int nextQuizID;
public:
    int quizID;
    string title;
    vector<Question*> questions;

    Quiz(string t = "Quiz") : title(t) { quizID = ++nextQuizID; }

    Question* addQuestion(string text, int points) {
        Question* q = new Question(text, points);
        questions.push_back(q);
        return q;
    }

    float calculateScore(const map<int, int>& userAnswers);
    void addResult(QuizResult* r) { (void)r; /* optional storage */ }
};

class StudentAnswer {
public:
    Answer* selectedAnswer;
    StudentAnswer(Answer* a) : selectedAnswer(a) {}
    Answer* getSelectedAnswer() { return selectedAnswer; }
};

class QuizResult {
private:
    float score;
    time_t dateTaken;
    vector<StudentAnswer*> studentAnswers;
public:
    QuizResult() : score(0), dateTaken(time(nullptr)) {}
    void setScore(float s) { score = s; }
    float getScore() const { return score; }
    time_t getDate() const { return dateTaken; }
    void addStudentAnswer(StudentAnswer* sa) { studentAnswers.push_back(sa); }
};

class Student : public Account {
public:
    string studentID;
    vector<QuizResult*> results;
    
    Student(string u = "", string p = "", string id = "") 
        : Account(u, p), studentID(id) { 
        role = "Student"; 
    }

    QuizResult* takeQuiz(Quiz* quiz) {
        map<int,int> answers;
        for (Question* q : quiz->questions) {
            cout << "Question: " << q->questionText << " (" << q->points << " pts)\n";
            int idx = 1;
            for (Answer* a : q->answers) {
                cout << "  " << idx << ") " << a->getText() << " (id=" << a->getID() << ")\n";
                idx++;
            }
            cout << "Choose answer index: "; 
            int sel; 
            cin >> sel;
            if (sel < 1 || sel > (int)q->answers.size()) sel = 1;
            answers[q->questionID] = q->answers[sel-1]->getID();
        }
        
        float score = quiz->calculateScore(answers);
        QuizResult* res = new QuizResult();
        res->setScore(score);
        
        for (auto& p : answers) {
            Question* q = nullptr;
            for (Question* qq : quiz->questions) {
                if (qq->questionID == p.first) { 
                    q = qq; 
                    break; 
                }
            }
            if (q) {
                for (Answer* a : q->answers) {
                    if (a->getID() == p.second) {
                        res->addStudentAnswer(new StudentAnswer(a));
                    }
                }
            }
        }
        results.push_back(res);
        return res;
    }

    void viewResult() {
        for (QuizResult* r : results) {
            time_t t = r->getDate();
            tm *tm_ptr = localtime(&t);
            cout << "Score: " << r->getScore() << "% taken on " 
                 << put_time(tm_ptr, "%c") << "\n";
        }
    }
};

class Admin : public Account {
public:
    string teacherID;
    
    Admin(string u = "", string p = "", string id = "") 
        : Account(u, p), teacherID(id) { 
        role = "Admin"; 
    }
    
    Quiz* makeQuiz(string title) { 
        return new Quiz(title); 
    }
    
    void manageQuestion(Quiz* quiz) {
        string text; 
        int pts;
        cout << "Enter question text: "; 
        cin.ignore(); 
        getline(cin, text);
        cout << "Points: "; 
        cin >> pts;
        
        Question* q = quiz->addQuestion(text, pts);
        cin.ignore();
        
        while (true) {
            string at; 
            cout << "Answer text (empty to stop): "; 
            getline(cin, at);
            if (at.empty()) break;
            
            char ch; 
            cout << "Is correct? (y/n): "; 
            cin >> ch; 
            cin.ignore();
            q->addAnswer(at, (ch == 'y' || ch == 'Y'));
        }
    }
    
    void viewResult(Quiz* quiz) { 
        /* not implemented in this simple sample */ 
    }
};

// Helper function
Question* getQuestionById(Quiz& quiz, int qid) {
    for (Question* q : quiz.questions) {
        if (q->questionID == qid) return q;
    }
    return nullptr;
}

// Load users from file
vector<Account*> loadUsers() {
    vector<Account*> users;
    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << "Error: Cannot open users.txt\n";
        return users;
    }
    string u, p, r;
    while (file >> u >> p >> r) {
        if (r == "Student") 
            users.push_back(new Student(u, p, u));
        else 
            users.push_back(new Admin(u, p, u));
    }
    return users;
}

// Load sample quiz
Quiz loadQuiz() {
    Quiz q("Basic Math Quiz");
    
    Question* q1 = q.addQuestion("What is 2 + 2?", 1);
    q1->addAnswer("1", false);
    q1->addAnswer("3", false);
    q1->addAnswer("4", true);

    Question* q2 = q.addQuestion("What is 5 * 6?", 2);
    q2->addAnswer("11", false);
    q2->addAnswer("30", true);
    q2->addAnswer("35", false);

    return q;
}

// Static counters initialization
int Question::nextQuestionID = 0;
int Answer::nextAnswerID = 0;
int Quiz::nextQuizID = 0;

// Method implementations
Answer* Question::addAnswer(string text, bool isCorrect) {
    Answer* a = new Answer(text, isCorrect);
    answers.push_back(a);
    return a;
}

bool Question::checkAnswer(int answerID) {
    for (Answer* a : answers) {
        if (a->getID() == answerID) return a->correct();
    }
    return false;
}

float Quiz::calculateScore(const map<int, int>& userAnswers) {
    int total = 0;
    int scored = 0;
    
    for (Question* q : questions) {
        total += q->points;
    }
    
    for (auto& p : userAnswers) {
        int qid = p.first; 
        int aid = p.second;
        Question* q = getQuestionById(*this, qid);
        if (q && q->checkAnswer(aid)) {
            scored += q->points;
        }
    }
    
    if (total == 0) return 0.0f;
    return 100.0f * scored / total;
}

// Main program
int main() {
    vector<Account*> users = loadUsers(); 
    Quiz quiz = loadQuiz();              
    
 
    cout << "=== QUIZ EXAMINATION SYSTEM ===\n";
    
    string u, p;  
    cout << "Username: ";
    cin >> u;
    cout << "Password: ";
    cin >> p;
    
    string role = "";       
    Account* auth = nullptr; 
    

    for (auto acc : users) {
        if (acc->username == u && acc->password == p) { 
            auth = acc;       
            role = acc->role; 
            break;           
        }
    }
    
    if (role == "") {
        cout << "Login failed! Wrong username or password.\n";
        return 0;  // Thoát chương trình
    }
    
    if (role == "Student") {
        Student* s = dynamic_cast<Student*>(auth);
        
        if (s) {
            QuizResult* r = s->takeQuiz(&quiz);
            
            cout << "\n=== RESULT ===\n";
            cout << "Your score: " << r->getScore() << "%\n";
        }
    }
    else if (role == "Admin") {
        cout << "\n=== ADMIN PANEL ===\n";
        cout << "Hello Admin " << u << "!" << endl;
        cout << "\nList of questions:\n";
        
        for (Question* q : quiz.questions) {
            cout << "- " << q->questionText << endl;
        }
    }
    
    return 0;  
}
