#include "filter.h"


string Filter::addFooter(string unfiltered)
{
    string filtered;
    unsigned int filter_index;

    filter_index = unfiltered.find("</body>", 0);    

    if (DEBUG) {
        if (filter_index != string::npos)
            cout << "found </body> at " << filter_index << endl;
        else {
            cout << "didn't find </body>" << endl;
        } 
    }

    unsigned int i;

    for (i = 0; i < filter_index; i++) {
        filtered.append(1, unfiltered.at(i));
    }

    filtered.append("<hr><p><h1>testing smth brooooooo</h1></p>");

    filtered.append("</ul><a href=\"/index.html\">return to main page</a>");
    filtered.append("</body></html>");

    return filtered;
}
