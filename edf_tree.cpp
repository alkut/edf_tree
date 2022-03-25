#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <fstream>

using namespace std;

typedef vector<pair<int, int>> vec;
typedef pair<int, int> p;

class edf_tree
{
    public:
  class node
  {
      public:
      int L;
      vec v;
      node *left=nullptr, *right=nullptr;
      vector<int> centr;
  };
  node *root;
  node *build(vec& arr, const vector<int>& sort_by_x, int l, int r) // arr осортирован по Y берём копию ищем медиану по X [,,M,,,] size/2 POINT - (x,y) бежим по arr P(x', y') x'
  {
      int new_r = l, new_l = r;
      if (arr.empty())
      {
          return nullptr;
      }
      node *tmp=new node;
      if (arr.size()==1)
      {
        tmp->v=std::move(arr);
        return tmp;
      }
//      auto copy = arr;
      vec v1, v2;
      vector<int> cen;
//      nth_element(copy.begin(), copy.begin() + copy.size()/2, copy.end());
      auto med = sort_by_x[(r+l) / 2];
      for (int i = 0; i < arr.size(); ++i)
      {
        if (arr[i].first < med)
        {
            v1.push_back(arr[i]);
            new_r++;
        }
        else if (arr[i].first > med)
        {
            v2.push_back(arr[i]);
            new_l--;
        }
        else
        {
            cen.push_back(arr[i].second);
        }
      }
      
      tmp->L=med;
      tmp->v=std::move(arr);
      tmp->centr = std::move(cen);
      tmp->left=build(v1, sort_by_x, l, new_r);
      tmp->right=build(v2, sort_by_x, new_l, r);
      return tmp;
      
  }
  public:
  edf_tree(const vec& arr, const vector<int>& sort_by_x)
  {
      vec tmp=arr;
      sort(tmp.begin(), tmp.end(), [](const p& p1, const p& p2)
      {
         return p1.second<p2.second;
      });
      root=build(tmp, sort_by_x, 0, arr.size());
  }
    void print()
    {
        print(root);
        return;
    }
    void print(node * ptr)
    {
        if(ptr == nullptr)
        {
            return;
        }
        cout << "med -" <<ptr->L << '\n';
        for (auto it : ptr->v)
        {
            cout << '(' << it.first << ", " << it.second << ") ";
        }
        cout << endl;
        print(ptr->left);
        print(ptr->right);
        return;
    }
  int qru(const p& point, node *tmp) const
  {
      if (tmp==nullptr) return 0;
      if (tmp->v.size()==0) return 0;
      if (tmp->v.size()==1)
      {
              if (tmp->v[0].first >= point.first && tmp->v[0].second >= point.second)
          return 1;
          return 0;
      }
      if (point.first > tmp->L)
      {
              return qru(point, tmp->right);
      }
      int aans = qru(point, tmp->left);
      
      if(tmp->right != nullptr)
      aans+=tmp->right->v.size()-(lower_bound(tmp->right->v.begin(), tmp->right->v.end(), point, [](const p& p1, const p& p2)
      {
          return p1.second < p2.second;
      })-tmp->right->v.begin());
      
      if (tmp->centr.size() != 0)
      {
          aans += tmp->centr.size() - (lower_bound(tmp->centr.begin(), tmp->centr.end(), point.second) - tmp->centr.begin());
      }
      // t(n) =  t(n/2) + 2 lg(n)
      return aans;
      
      }
   int qru(const p& point) const
  {
      return qru(point, root);
  }
  
    int qrd(const p& point) const
  {
      return qrd(point, root);
  }
  int qrd(const p& point, node *tmp) const
  {
      if (tmp==nullptr) return 0;
      if (tmp->v.size()==0) return 0;
      if (tmp->v.size()==1)
      {
              if (tmp->v[0].first >= point.first && tmp->v[0].second <= point.second)
          return 1;
          return 0;
      }
      if (point.first > tmp->L) // наша точка справа
      {
              return qrd(point, tmp->right);
      }
      int aans = qrd(point, tmp->left);
      if(tmp->right != nullptr)
      aans+=upper_bound(tmp->right->v.begin(), tmp->right->v.end(), point,  [](const p& p1, const p& p2)
      {
          return p1.second < p2.second;
      })-tmp->right->v.begin();
      
      if (tmp->centr.size() != 0)
      {
          aans += (upper_bound(tmp->centr.begin(), tmp->centr.end(), point.second) - tmp->centr.begin());
      }
          
      return aans;
  }
  
    int qld(const p& point) const
  {
      return qld(point, root);
  }
    int qld(const p& point, node *tmp) const// левее ниже
  {
      if (tmp==nullptr) return 0;
      if (tmp->v.size()==0) return 0;
      if (tmp->v.size()==1)
      {
              if (tmp->v[0].first <= point.first && tmp->v[0].second <= point.second)
          return 1;
          return 0;
      }
      if (point.first < tmp->L) // если наша точка левее X * M * *
      {
              return qld(point, tmp->left); // ищем слева
      }
      int aans = qld(point, tmp->right); // * * * M X * * * нам подходят все слева
      if (tmp->left != nullptr)
      aans+=upper_bound(tmp->left->v.begin(), tmp->left->v.end(), point, [](const p& p1, const p& p2)
      {
          return p1.second < p2.second;
      })-tmp->left->v.begin();
      if (tmp->centr.size() != 0)
      {
          aans += (upper_bound(tmp->centr.begin(), tmp->centr.end(), point.second) - tmp->centr.begin());
      }
      
      return aans;
  }
  
    int qlu(const p& point)
  {
      return qlu(point, root);
  }
  
    int qlu(const p& point, node *tmp)
  {
      if (tmp==nullptr) return 0;
      if (tmp->v.size()==0) return 0;
      if (tmp->v.size()==1)
      {
              if (tmp->v[0].first <= point.first && tmp->v[0].second >= point.second)
          return 1;
          return 0;
      }
      if (point.first < tmp->L) // идём налево
      {
              return qlu(point, tmp->left);
      }
      int aans = qlu(point, tmp->right); // заходим направо и берём все слева
      
      if (tmp->left != nullptr)
      aans+=tmp->left->v.size()-(lower_bound(tmp->left->v.begin(), tmp->left->v.end(),point, [](const p& p1, const p& p2)
      {
          return p1.second < p2.second;
      })-tmp->left->v.begin());
      if (tmp->centr.size() != 0)
      {
          aans += tmp->centr.size() - (lower_bound(tmp->centr.begin(), tmp->centr.end(), point.second) - tmp->centr.begin());
      }
      return aans;
  
  }
};



int main()
{

    return 0;
}

