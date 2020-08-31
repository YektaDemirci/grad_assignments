import pandas as pd
import numpy as np
import seaborn as sns

from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import matplotlib.pyplot as plt
from sklearn import preprocessing

#ISCXVPN2016
#self
option='ISCXVPN2016'
data = np.loadtxt("/home/yekta/Desktop/CS_656_Project/"+option+"_uni_Data.csv",delimiter=',')

data_http=[]
http2=[]
http1=[]
types=[[],[],[],[]]
for flw in data:
    #Ignore outliers
    if flw[0] > 2000:
        continue
    else:
        if flw[5] == 1:
            flw[5]=0
            data_http.append(flw)
            http1.append(flw)
            if flw[4]==1:
                types[0].append(flw)
            if flw[4]==2:
                types[1].append(flw)
        if flw[5] == 2:
            flw[5]=1
            data_http.append(flw)
            http2.append(flw)
            if flw[4]==1:
                types[2].append(flw)
            if flw[4]==2:
                types[3].append(flw)
        


labels = 'HTTP1','HTTP2'
sizes = [len(http1),len(http2)]

fig1, ax1 = plt.subplots()
ax1.pie(sizes, labels=labels, autopct='%1.1f%%',shadow=True)
ax1.axis('equal')
plt.title("Distribution of the HTTP protocols: "+option+" dataset")
plt.show()

types_pd=[]
for i in types:
    types_pd.append(pd.DataFrame(np.array(i),columns=['Packet','Byte','BytePerPacket','Time','CS','Protocol']))

features = ['Packet','Byte','BytePerPacket','Time','CS','Protocol']
labels2 = 'HTTP1_Client', 'HTTP1_Server', 'HTTP2_Client', 'HTTP2_Server'      

feature=features[3]
maxx=0
for i in range(0,4):
    if max(types_pd[i][feature]) > maxx:
        maxx=max(types_pd[i][feature])
        print(maxx)
maxx=int(maxx)
#If there is any outlier
#maxx=200

time=[i for i in range(maxx)]

for i in range(0,4):
    p=[0]*maxx
    
    sortedtime = np.sort(types_pd[i][feature])
    sortedtime = sortedtime.tolist()
    sortedtime = [int(number) for number in sortedtime]
    
    total=0
    for j in range(0,maxx):
        total=sortedtime.count(j)+total
        p[j]=total
    prob = [number / total for number in p]
    
    while(len(p)< maxx):
        p.append(1)
    plt.plot(time,prob, label=labels2[i])
plt.xscale('log')
plt.xlabel(feature)
plt.ylabel("CDF")
plt.title(option+" data CDF of "+feature)
plt.legend()
plt.show()


http_data=pd.DataFrame(np.array(data_http),columns=['Packet','Byte','BytePerPacket','Time','CS','Protocol'])
F=['Packet','Byte','BytePerPacket','Time']
L='Protocol'
FL=F+[L]
sns.pairplot(http_data[FL], vars = F, hue=L, palette='colorblind', diag_kind='hist');

X = http_data[F].values
y = np.ravel(http_data[[L]])

ran = 42
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state = ran)

scaler = preprocessing.StandardScaler(with_mean=True, with_std=True)
X_train=scaler.fit_transform(X_train)

###############################################################################
#PCA
from sklearn.decomposition import PCA

scaler = preprocessing.StandardScaler(with_mean=True, with_std=True)
data_scaled = scaler.fit_transform(X)

pca = PCA()
pca.fit(data_scaled)

eigenVals = pca.explained_variance_
eigenVec = pca.components_
indx = [i for i in range(4)]
plt.plot(indx, eigenVals, 'ro', markersize=2)

data_pc=data_scaled.dot(pca.components_[:,:].T)
col=['PC'+str(i+1) for i in range(4)]
label = np.array(y)
data_pc=pd.DataFrame(np.array(X),columns=col)
data_pc = data_pc.assign(label=label)


fig, axs = plt.subplots(3)
fig.set_size_inches(6.4, 4.8*3)
for i in range(0,3):
    #axs[i].plot(data_pc["PC"+str(i+1)], data_pc["PC"+str(i+2)],'ro', markersize=2)
    sns.scatterplot(x=data_pc["PC"+str(i+1)], y=data_pc["PC"+str(i+2)],hue=data_pc["label"],ax=axs[i],palette='colorblind')
    axs[i].set_title('PC'+str(i+1)+" vs PC"+str(i+2))

###############################################################################
#LDA
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA

lda = LDA()
lda.fit_transform(X,y)
data_lda = X.dot(lda.scalings_[:,:])

col=['Dim1']

data_lda=pd.DataFrame(np.array(data_lda),columns=col)
#Since there are 2 labels, LDA will return a vector, to visualize it in 2D we need another vector
rndm = pd.DataFrame(np.random.randn(len(data_http), 1), columns=['AxisRand'])

data_lda = data_lda.assign(AxisRand=rndm)
data_lda = data_lda.assign(label=y)
sns.scatterplot(x=data_lda['Dim1'], y=data_lda['AxisRand'],hue=data_lda["label"],palette='colorblind')


###############################################################################
#KNN implementation
from sklearn import neighbors
from sklearn.model_selection import cross_val_score

F=['Packet','Byte','BytePerPacket','Time']
L='Protocol'

X = http_data[F].values
y = np.ravel(http_data[[L]])

ran = 42
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state = ran)
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

#Number of neighbors
start=1
end=31
n_neighborslist = list(range(start,end))
col_names=['euclidean','manhattan','chebyshev']
accarray = np.zeros((len(n_neighborslist),len(col_names)))

#add multiple plots to same chart, one for each weighting approach
indx = [i for i in range(start,end)]
acc=pd.DataFrame(accarray, columns=col_names,index=indx)
acc
for k in n_neighborslist:
    for i in range(0,len(col_names)):
        neigh = neighbors.KNeighborsClassifier(n_neighbors=k, weights='distance', metric=col_names[i])
        cv_scores = cross_val_score(neigh, X_train, y_train, cv=10)
        acc.at[k,col_names[i]] = cv_scores.mean()

ax=acc[:].plot.line();
ax.set_ylabel("Accuracy")
ax.set_xlabel("Number of neighbours")

# Making the Confusion Matrix with the best parameters
from sklearn.metrics import confusion_matrix


neigh = neighbors.KNeighborsClassifier(n_neighbors=3, weights='distance', metric='manhattan')
neigh.fit(X_train, y_train)
y_pred = neigh.predict(X_test)
accscore = accuracy_score(y_test, y_pred)
cm = confusion_matrix(y_test, y_pred)
a=cm[0][0]
b=cm[0][1]
c=cm[1][0]
d=cm[1][1]

cm=cm.astype(float)
cm[0][0]=round( (a/(a+b))*100 , 2)
cm[0][1]=round( (b/(a+b))*100 , 2)
cm[1][0]=round( (c/(c+d))*100 , 2)
cm[1][1]=round( (d/(c+d))*100 , 2)

strings = np.asarray([[str(cm[0][0])+'%', str(cm[0][1])+'%'],
                      [str(cm[1][0])+'%', str(cm[1][1])+'%']])

ax = plt.axes()
sns.heatmap(cm,cmap="Greys",annot=strings ,fmt = '',cbar=False,xticklabels=["HTTP1","HTTP2"], yticklabels=["HTTP1","HTTP2"])
plt.title('Confusion matrix of KNN')
plt.show()

###############################################################################
#Support vector implementation
from sklearn.svm import SVC
from sklearn.model_selection import cross_val_score

F=['Packet','Byte','BytePerPacket','Time']
L='Protocol'
X = http_data[F].values
y = np.ravel(http_data[[L]])

ran = 42
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state = ran)
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

parameters=[0.1, 0.5, 1, 2, 5, 10, 20, 50, 60]
parameters2=['0.1', '0.5', '1', '2', '5', '10', '20', '50','60']


kernels=['linear', 'poly', 'rbf', 'sigmoid']

accarray = np.zeros( (len(parameters),len(kernels) ) )
acc=pd.DataFrame(accarray, columns=kernels,index=parameters2)

for k in range(0,len(parameters)):
    for i in range(0,len(kernels)):
        classifier = SVC(kernel=kernels[i], C=parameters[k], random_state=42)
        fins=cross_val_score(classifier,X_train,y_train, cv=10)
        acc.at[parameters2[k],kernels[i]] = fins.mean()

ax=acc.plot.line();
ax.set_ylabel("Accuracy")
ax.set_xlabel("Regularization parameter")

# Making the Confusion Matrixwith the best parameters
from sklearn.metrics import confusion_matrix

classifier = SVC(kernel='rbf', C=50, random_state=42)
classifier.fit(X_train, y_train)
y_pred = classifier.predict(X_test)
accscore = accuracy_score(y_test, y_pred)
cm = confusion_matrix(y_test, y_pred)
a=cm[0][0]
b=cm[0][1]
c=cm[1][0]
d=cm[1][1]

cm=cm.astype(float)
cm[0][0]=round( (a/(a+b))*100 , 2)
cm[0][1]=round( (b/(a+b))*100 , 2)
cm[1][0]=round( (c/(c+d))*100 , 2)
cm[1][1]=round( (d/(c+d))*100 , 2)

strings = np.asarray([[str(cm[0][0])+'%', str(cm[0][1])+'%'],
                      [str(cm[1][0])+'%', str(cm[1][1])+'%']])

ax = plt.axes()
sns.heatmap(cm,cmap="Greys",annot=strings ,fmt = '',cbar=False,xticklabels=["HTTP1","HTTP2"], yticklabels=["HTTP1","HTTP2"])
plt.title('Confusion matrix of SVC')
plt.show()

###############################################################################
#Decision tree implementation
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import cross_val_score

F=['Packet','Byte','BytePerPacket','Time']
L='Protocol'
X = http_data[F].values
y = np.ravel(http_data[[L]])

ran = 42
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state = ran)
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

column=['standard']
parameters=[3, 5, 7, 10, 'UntilEnd']
parameters2=['3', '5','7', '10', 'UntilEnd']


accarray = np.zeros( (len(parameters),len(column) ) )
acc=pd.DataFrame(accarray, columns=column,index=parameters2)

for k in range(0,len(parameters)):
    if k!=4:
        classifier = DecisionTreeClassifier(max_depth=parameters[k], random_state = 42)
    else:
        classifier = DecisionTreeClassifier(random_state = 42)
    fins=cross_val_score(classifier,X_train,y_train, cv=10)
    acc.at[parameters2[k],column[0]] = round(fins.mean(),4)

acc.plot.line();
ax.set_ylabel("Accuracy")
ax.set_xlabel("Depthness")

# Making the Confusion Matrixwith the best parameters
from sklearn.metrics import confusion_matrix

classifier = DecisionTreeClassifier(max_depth=7, random_state = 42)
classifier.fit(X_train, y_train)
y_pred = classifier.predict(X_test)
accscore = accuracy_score(y_test, y_pred)
cm = confusion_matrix(y_test, y_pred)
a=cm[0][0]
b=cm[0][1]
c=cm[1][0]
d=cm[1][1]

cm=cm.astype(float)
cm[0][0]=round( (a/(a+b))*100 , 2)
cm[0][1]=round( (b/(a+b))*100 , 2)
cm[1][0]=round( (c/(c+d))*100 , 2)
cm[1][1]=round( (d/(c+d))*100 , 2)

strings = np.asarray([[str(cm[0][0])+'%', str(cm[0][1])+'%'],
                      [str(cm[1][0])+'%', str(cm[1][1])+'%']])

ax = plt.axes()
sns.heatmap(cm,cmap="Greys",annot=strings ,fmt = '',cbar=False,xticklabels=["HTTP1","HTTP2"], yticklabels=["HTTP1","HTTP2"])
plt.title('Confusion matrix of Decision Tree')
plt.show()

###############################################################################
#Random forest implementation
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import cross_val_score
import seaborn as sns

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

trees=[5, 10, 50, 100, 150, 200]
parameters=[3, 5, 7 ,10, 'UntilEnd']
parameters2=['3', '5','7', '10', 'UntilEnd']

accarray = np.zeros( ( len(trees), len(parameters2) ) ) 
avg=pd.DataFrame(accarray, columns=parameters2, index=trees)

for t in range(0,len(trees)):
    for k in range(0,len(parameters)):
        if k!=4:
            classifier = RandomForestClassifier(n_estimators=trees[t],max_depth=parameters[k], random_state = 42)
        else:
            classifier = RandomForestClassifier(n_estimators=trees[t],random_state = 42)
        fins=cross_val_score(classifier,X_train,y_train, cv=10)
        avg.at[trees[t],parameters2[k]] = fins.mean()

ax = plt.axes()
sns.heatmap(avg,cmap="Greys",annot=True,xticklabels=parameters2, yticklabels=trees,ax=ax)
ax.set(xlabel='Depthness values', ylabel='Number of trees', title='Random forest accuracy based on depth')
plt.show()



# Making the Confusion Matrixwith the best parameters
from sklearn.metrics import confusion_matrix

classifier = RandomForestClassifier(n_estimators=150,max_depth=10, random_state = 42)
classifier.fit(X_train, y_train)
y_pred = classifier.predict(X_test)
accscore = accuracy_score(y_test, y_pred)
cm = confusion_matrix(y_test, y_pred)
a=cm[0][0]
b=cm[0][1]
c=cm[1][0]
d=cm[1][1]

cm=cm.astype(float)
cm[0][0]=round( (a/(a+b))*100 , 2)
cm[0][1]=round( (b/(a+b))*100 , 2)
cm[1][0]=round( (c/(c+d))*100 , 2)
cm[1][1]=round( (d/(c+d))*100 , 2)

strings = np.asarray([[str(cm[0][0])+'%', str(cm[0][1])+'%'],
                      [str(cm[1][0])+'%', str(cm[1][1])+'%']])

ax = plt.axes()
sns.heatmap(cm,cmap="Greys",annot=strings ,fmt = '',cbar=False,xticklabels=["HTTP1","HTTP2"], yticklabels=["HTTP1","HTTP2"])
plt.title('Confusion matrix of Random Forest')
plt.show()
###############################################################################
from keras.wrappers.scikit_learn import KerasClassifier
from sklearn.model_selection import GridSearchCV
from keras.models import Sequential
from keras.layers import Dense
def build_classifier(optimizer):
    classifier = Sequential()
    classifier.add(Dense(units = 4, kernel_initializer = 'uniform', activation = 'relu', input_dim = 4))
    classifier.add(Dense(units = 4, kernel_initializer = 'uniform', activation = 'relu'))
    classifier.add(Dense(units = 1, kernel_initializer = 'uniform', activation = 'sigmoid'))
    classifier.compile(optimizer = optimizer, loss = 'binary_crossentropy', metrics = ['accuracy'])
    return classifier
classifier = KerasClassifier(build_fn = build_classifier)
parameters = {'batch_size': [25, 32],
              'epochs': [100, 200],
              'optimizer': ['adam', 'rmsprop']}
grid_search = GridSearchCV(estimator = classifier,
                           param_grid = parameters,
                           scoring = 'accuracy',
                           cv = 10)
grid_search = grid_search.fit(X_train, y_train)
best_parameters = grid_search.best_params_
best_accuracy = grid_search.best_score_

#Metrics are found

import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import Dropout

scaler = preprocessing.StandardScaler(with_mean=True, with_std=True)

F=['Packet','Byte','BytePerPacket','Time']
L='Protocol'
X = http_data[F].values
y = np.ravel(http_data[[L]])

ran = 42
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state = ran)
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

# Initialising the ANN
classifier = Sequential()

classifier.add(Dense(units = 6, kernel_initializer = 'uniform', activation = 'relu', input_dim = 4))
classifier.add(Dense(units = 6, kernel_initializer = 'uniform', activation = 'relu'))
classifier.add(Dense(units = 1, kernel_initializer = 'uniform', activation = 'sigmoid'))

classifier.compile(optimizer = 'adam', loss = 'binary_crossentropy', metrics = ['accuracy'])
classifier.fit(X_train, y_train, batch_size = 25, epochs = 200)

thresholds=[0.1 ,0.2 ,0.3 ,0.4 ,0.5 ,0.6 ,0.7 ,0.8 ,0.9]
for thrs in thresholds:
    y_pred = classifier.predict(X_test)
    y_pred = (y_pred > thrs)
    cm = confusion_matrix(y_test, y_pred)
    print(cm)

y_pred = classifier.predict(X_test)
y_pred = (y_pred > 0.5)
cm = confusion_matrix(y_test, y_pred)
print(cm)

a=cm[0][0]
b=cm[0][1]
c=cm[1][0]
d=cm[1][1]

cm=cm.astype(float)
cm[0][0]=round( (a/(a+b))*100 , 2)
cm[0][1]=round( (b/(a+b))*100 , 2)
cm[1][0]=round( (c/(c+d))*100 , 2)
cm[1][1]=round( (d/(c+d))*100 , 2)

strings = np.asarray([[str(cm[0][0])+'%', str(cm[0][1])+'%'],
                      [str(cm[1][0])+'%', str(cm[1][1])+'%']])

ax = plt.axes()
sns.heatmap(cm,cmap="Greys",annot=strings ,fmt = '',cbar=False,xticklabels=["HTTP1","HTTP2"], yticklabels=["HTTP1","HTTP2"])
plt.title('Confusion matrix of Neural Network')
plt.show()







