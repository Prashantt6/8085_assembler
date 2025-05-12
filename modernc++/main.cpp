#include<iostream>
#include<vector>
#include<functional>
// void applyToNumbers(int x){
//     std::cout<< x<<std:: endl;
// }
// void applyToNumbers(const std::vector<int>& nums, std:: function<void(int)> func){
//     for( int val : nums)
//     {
//      func(val);
//     }
// }
std:: vector<int> transform(const std::vector<int>& nums, std:: function<int(int)> func){
    std:: vector<int>result;
    for(int val : nums){
        result.push_back(func(val));
    }
}
int main(){
   
   std:: vector<int> nums = {1,2,4,5};
   
//    applyToNumbers(nums , [](int x) {
//        std:: cout<<"SQUARE"<<x*x<<std:: endl;
//    });
transform(nums,[](int x){return x*x*x});
 

}